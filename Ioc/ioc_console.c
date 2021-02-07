/*
 * CONSOLE UART - SCN2661 Enhanced Programmable communications interface
 * ---------------------------------------------------------------------
 *
 * Chip Select: 0xffff9xxx
 *
 */

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "r1000.h"
#include "ioc.h"
#include "elastic.h"

static pthread_mutex_t	cons_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t	cons_cond = PTHREAD_COND_INITIALIZER;
static pthread_t	cons_rx;
static pthread_t	cons_tx;

static struct cons {
	struct elastic	*ep;
	uint8_t		mode[2];
	int		mptr;
	uint8_t		cmd;
	uint8_t		status;
	uint8_t		rxhold;
	uint8_t		txhold;
	unsigned	updated;
} cons[1];

void v_matchproto_(cli_func_f)
cli_ioc_console(struct cli *cli)
{

	if (cli->help) {
		cli_io_help(cli, "IOC console", 0, 1);
		return;
	}

	cli->ac--;
	cli->av++;

	while (cli->ac && !cli->status) {
		if (cli_elastic(cons->ep, cli))
			continue;
		if (cli->ac >= 1 && !strcmp(cli->av[0], "test")) {
			elastic_put(cons->ep, "Hello World\r\n", -1);
			cli->ac -= 1;
			cli->av += 1;
			continue;
		}
		cli_unknown(cli);
		break;
	}
}

/**********************************************************************/

static void*
thr_console_rx(void *priv)
{
	uint8_t buf[1];
	ssize_t sz;

	(void)priv;
	while (1) {
		sz = elastic_get(cons->ep, buf, 1);
		assert(sz == 1);

		AZ(pthread_mutex_lock(&cons_mtx));
		cons->rxhold = buf[0];
		cons->status |= 2;
		AZ(pthread_mutex_unlock(&cons_mtx));
	}
}

/**********************************************************************/

static void*
thr_console_tx(void *priv)
{
	(void)priv;

	AZ(pthread_mutex_lock(&cons_mtx));
	while (1) {
		if (!cons->updated) {
			AZ(pthread_cond_wait(
			    &cons_cond, &cons_mtx));
		}
		cons->updated = 0;
		if (!(cons->status & 1)) {		// tx-hold was filled
			switch (cons->cmd & 0xc0) {
			case 0x00: // Normal mode
				elastic_put(
				    cons->ep,
				    &cons->txhold,
				    1
				);
				break;
			case 0x80: // Local Loopback mode
				cons->rxhold = cons->txhold;
				cons->status |= 2;
				break;
			default:
				break;
			}
			cons->status |= 1;		// tx-hold empty
			cons->status |= 4;		// tx-shift empty
		}
	}
}

/**********************************************************************/

unsigned int v_matchproto_(iofunc_f)
io_console_uart(
    const char *op,
    unsigned int address,
    memfunc_f *func,
    unsigned int value
)
{

	(void)func;
	IO_TRACE_WRITE(2, "CONSOLE_UART");

	AZ(pthread_mutex_lock(&cons_mtx));
	switch (address & 3) {
	case 0x00:	// Data
		if (op[0] == 'W') {
			cons->txhold = value;
			cons->status &= ~1;		// tx-hold busy
			cons->status &= ~4;		// tx-shift full
		} else {
			value = cons->rxhold;
			cons->rxhold = 0;
			cons->status &= ~2;		// rx-hold empty
		}
		break;
	case 0x01:	// Status
		value = 0;
		if (op[0] == 'R')
			value = cons->status;
		break;
	case 0x02:	// Mode
		if (op[0] == 'W')
			cons->mode[cons->mptr] = value;
		else
			value = cons->mode[cons->mptr];
		cons->mptr ^= 1;
		break;
	case 0x03:	// Command
		if (op[0] == 'W') {
			cons->cmd = value;
			if (value & 0x1)
				cons->status |= 1;	// tx-hold empty
		} else {
			value = cons->cmd;
		}
		break;
	default:
		break;
	}
	if (op[0] == 'W') {
		cons->updated = 1;
		AZ(pthread_cond_signal(&cons_cond));
	}

	AZ(pthread_mutex_unlock(&cons_mtx));
	IO_TRACE_READ(2, "CONSOLE_UART");

	return (value);
}

void
ioc_console_init(struct sim *sim)
{

	cons->ep = elastic_new(sim, O_RDWR);
	AZ(pthread_create(&cons_rx, NULL, thr_console_rx, NULL));
	AZ(pthread_create(&cons_tx, NULL, thr_console_tx, NULL));
}
