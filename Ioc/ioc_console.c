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

static const char * const rd_reg[] = {"DATA", "STATUS", "MODE", "CMD"};
static const char * const wr_reg[] = {"DATA", "SYN/DLE", "MODE", "CMD"};

#define REG_R_DATA	0
#define REG_R_STATUS	1
#define REG_R_MODE	2
#define REG_R_CMD	3

#define REG_W_DATA	0
#define REG_W_SYN_DLE	1
#define REG_W_MODE	2
#define REG_W_CMD	3

static pthread_mutex_t	cons_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t	cons_cond = PTHREAD_COND_INITIALIZER;
static pthread_t	cons_rx;

static struct cons {
	struct elastic	*ep;
	uint8_t		rdregs[4];
	uint8_t		wrregs[4];
	uint8_t		mode[2];
	int		mptr;
	uint8_t		cmd;
	uint8_t		status;
	uint8_t		rxhold;
	uint8_t		txhold;
	uint8_t		txshift;
	uint8_t		txshiftfull;
	int		txbreak;
	int		loopback;
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
		while (!(cons->cmd & 0x04) && (cons->status & 0x02))
			AZ(pthread_cond_wait(&cons_cond, &cons_mtx));
		cons->rxhold = buf[0];
		cons->status |= 0x02;
		irq_raise(&IRQ_CONSOLE_RXRDY);
		AZ(pthread_mutex_unlock(&cons_mtx));
	}
}

/**********************************************************************/

static void
cons_txshift_done(void * priv)
{
	(void)priv;

	AZ(pthread_mutex_lock(&cons_mtx));
	if (!(cons->cmd & 0x01)) {			// Tx disabled
		cons->status &= ~0x04;
		irq_lower(&IRQ_CONSOLE_TXRDY);
	} else if (cons->loopback && cons->txbreak) {
		cons->status |= 0x22;		// Break detected
		irq_raise(&IRQ_CONSOLE_BREAK);
	} else {
		if (cons->loopback && !cons->txbreak && cons->txshiftfull) {
			cons->rxhold = cons->txshift;
			cons->status |= 0x02;
			irq_raise(&IRQ_CONSOLE_RXRDY);
		} else if (cons->txshiftfull) {
			elastic_put(cons->ep, &cons->txhold, 1);
		}
		cons->txshiftfull = 0;
		if (cons->status & 0x01) {			// txhold is empty
			cons->status |= 0x04;			// TxEmt
		} else {
			cons->txshift = cons->txhold;
			cons->txshiftfull = 1;
			callout_callback(r1000sim, cons_txshift_done, NULL, 1000, 0);
			cons->status |= 0x01;			// txhold is empty
			irq_raise(&IRQ_CONSOLE_TXRDY);
		}
	}
	AZ(pthread_mutex_unlock(&cons_mtx));
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
	unsigned reg, chg;

	reg = address & 3;
	AZ(pthread_mutex_lock(&cons_mtx));
	if (op[0] == 'W') {
		trace(TRACE_IO, "CONSOLE %08x %s %08x %x %s\n",
		    ioc_pc, op, address, value, wr_reg[reg]);
		(void)func(op, cons->wrregs, reg, value);
		switch(reg) {
		case REG_W_DATA:
			cons->txhold = cons->wrregs[reg];
			cons->status &= ~1;		// tx-hold busy
			cons->status &= ~4;		// tx-shift full
			irq_lower(&IRQ_CONSOLE_TXRDY);
			break;
		case REG_W_MODE:
			cons->mode[cons->mptr] = cons->wrregs[reg];
			cons->mptr ^= 1;
			break;
		case REG_W_CMD:
			chg = cons->wrregs[reg] ^ cons->cmd;

			if ((chg & 0x01) && !(cons->cmd & 0x01)) {
				cons->cmd |= 0x01;	// tx-enabled
				cons->status |= 0x01;	// tx-hold empty
				irq_raise(&IRQ_CONSOLE_TXRDY);
			} else if ((chg & 0x01) && (cons->cmd & 0x01)) {
				cons->cmd &= ~0x01;	// tx-disabled
				cons->status &= ~0x05;	// tx-hold empty
				irq_lower(&IRQ_CONSOLE_TXRDY);
			}

			if ((chg & 0x04) && !(cons->cmd & 0x04)) {
				cons->cmd |= 0x04;	// rx-enabled
				cons->status &= ~0x02;	// rx-hold empty
			} else if ((chg & 0x04) && (cons->cmd & 0x04)) {
				cons->cmd &= ~0x04;	// rx-disabled
				cons->status &= ~0x02;	// rx-hold empty
				irq_lower(&IRQ_CONSOLE_RXRDY);
			}

			if (cons->wrregs[reg] & 0x10) {
				cons->status &= ~0x38;
				irq_lower(&IRQ_CONSOLE_BREAK);
			}

			cons->cmd &= ~0xea;
			cons->cmd |= cons->wrregs[reg] & 0xea;

			if ((chg & 0x08) && !cons->txbreak) {
				cons->txbreak = 1;
				cons->status &= ~0x05;	// tx-hold empty
				irq_lower(&IRQ_CONSOLE_TXRDY);
			} else if ((chg & 0x08) && cons->txbreak) {
				cons->txbreak = 0;
				cons->status |= 0x01;	// tx-hold empty
				irq_raise(&IRQ_CONSOLE_TXRDY);
				irq_lower(&IRQ_CONSOLE_BREAK);
			}
			cons->loopback = (cons->cmd & 0xc0) == 0x80;
			break;
		default:
			break;
		}
		cons->updated = 1;
		AZ(pthread_cond_broadcast(&cons_cond));
	} else {
		switch(reg) {
		case REG_R_DATA:
			cons->rdregs[reg] = cons->rxhold;
			cons->status &= ~2;		// rx-hold empty
			irq_lower(&IRQ_CONSOLE_RXRDY);
			break;
		case REG_R_STATUS:
			cons->rdregs[reg] = cons->status;
			break;
		case REG_R_MODE:
			cons->rdregs[reg] = cons->mode[cons->mptr];
			cons->mptr ^= 1;
			break;
		case REG_R_CMD:
			cons->rdregs[reg] = cons->cmd;
			break;
		default:
			break;
		}
		value = func(op, cons->rdregs, reg, value);
		trace(TRACE_IO, "CONSOLE %08x %s %08x %x %s\n",
		    ioc_pc, op, address, value, rd_reg[reg]);
	}
	AZ(pthread_mutex_unlock(&cons_mtx));
	if (op[0] == 'W') {
		if (cons->txbreak && cons->loopback)
			cons_txshift_done(NULL);
		else if (!(cons->status & 1) && !cons->txshiftfull)
			cons_txshift_done(NULL);
	}
	return (value);
}

void
ioc_console_init(struct sim *sim)
{

	cons->ep = elastic_new(sim, O_RDWR);
	AZ(pthread_create(&cons_rx, NULL, thr_console_rx, NULL));
}
