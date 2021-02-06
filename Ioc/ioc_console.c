/*
 * CONSOLE UART - SCN2661 Enhanced Programmable communications interface
 * ---------------------------------------------------------------------
 *
 * Chip Select: 0xffff9xxx
 *
 */

#include <fcntl.h>
#include <string.h>

#include "r1000.h"
#include "ioc.h"
#include "elastic.h"

static pthread_mutex_t	ioc_console_mtx;
static pthread_t	ioc_console_rx;


struct ioc_console {
	struct elastic	*ep;
	uint8_t		mode[2];
	int		mptr;
	uint8_t		cmd;
	uint8_t		data;
	uint8_t		status;
	uint8_t		rxhold;
	uintmax_t	next;
} ioc_console[1];

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
		if (cli_elastic(ioc_console->ep, cli))
			continue;
		if (cli->ac >= 1 && !strcmp(cli->av[0], "test")) {
			elastic_put(ioc_console->ep, "Hello World\r\n", -1);
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
dev_ptr_thread(void *priv)
{
	uint8_t buf[1];
	ssize_t sz;

	(void)priv;
	while (1) {
		sz = elastic_get(ioc_console->ep, buf, 1);
		assert(sz == 1);

		AZ(pthread_mutex_lock(&ioc_console_mtx));
		ioc_console->rxhold = buf[0];
		ioc_console->status |= 2;
		AZ(pthread_mutex_unlock(&ioc_console_mtx));
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
	uint8_t chr;

	(void)func;
	IO_TRACE_WRITE(2, "CONSOLE_UART");

	AZ(pthread_mutex_lock(&ioc_console_mtx));
	switch (address & 3) {
	case 0x00:	// Data
		if (op[0] == 'W') {
			chr = value;
			if (!(ioc_console->cmd & 0xc0)) {
				elastic_put(ioc_console->ep, &chr, 1);
			} else {
				ioc_console->rxhold = chr;
				ioc_console->status |= 2;
			}
			ioc_console->status &= ~1;
			ioc_console->next = ioc_nins + 20;
			ioc_console->data = value;
		} else {
			value = ioc_console->rxhold;
			ioc_console->rxhold = 0;
			ioc_console->status &= ~2;
		}
		break;
	case 0x01:	// Status
		value = 0;
		if (op[0] == 'W') {
			;
		} else {
			if ((ioc_console->status & 0x5) == 1)
				ioc_console->status |= 4;
			if (ioc_nins >= ioc_console->next) {
				ioc_console->next = (1ULL<<63);
				ioc_console->status |= 1;
			}
			value = ioc_console->status;
			ioc_console->status &= ~4;
		}
		break;
	case 0x02:	// Mode
		if (op[0] == 'W')
			ioc_console->mode[ioc_console->mptr] = value;
		else
			value = ioc_console->mode[ioc_console->mptr];
		ioc_console->mptr ^= 1;
		break;
	case 0x03:	// Command
		if (op[0] == 'W')
			ioc_console->cmd = value;
		else
			value = ioc_console->cmd;
		break;
	default:
		WRONG();
	}

	AZ(pthread_mutex_unlock(&ioc_console_mtx));
	IO_TRACE_READ(2, "CONSOLE_UART");

	return (value);
}

void
ioc_console_init(struct sim *sim)
{

	ioc_console->ep = elastic_new(sim, O_RDWR);
	AZ(pthread_mutex_init(&ioc_console_mtx, NULL));
	AZ(pthread_create(&ioc_console_rx, NULL, dev_ptr_thread, NULL));
}
