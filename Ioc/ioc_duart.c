/*
 * MODEM/DIAG DUART - SCN2681 Dual asynchronous receiver/transmitter
 * -----------------------------------------------------------------
 *
 * Chip Select: 0xffffaxxx
 *
 */

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "r1000.h"
#include "ioc.h"
#include "elastic.h"

static const char * const rd_reg[] = {
	"MR1A_2A", "SRA", "BRG Test", "RHRA", "IPCR", "ISR", "CTU", "CLL",
	"MR1B_2B", "SRB", "1x/16x Test", "RHRB", "Reserved", "Input Port",
	"Start Counter Command", "Stop Counter Command",
};

static const char * const wr_reg[] = {
	"MR1A_2A", "CSRA", "CRA", "THRA", "ACR", "IMR", "CRUR", "CTLR",
	"MR1B_2B", "CSRB", "CRB", "THRB", "Reserved", "OPCR",
	"Set Output Port Bits Command", "Reset Output Port Bits Command",
};

#define DUART_TRACE_WRITE(level, prefix)				\
	do {								\
		if (*op == 'W')						\
			trace(level, prefix " %08x %s %08x %x %s\n",	\
			    ioc_pc, op, address, value, wr_reg[address&0xf]);		\
	} while(0)

#define DUART_TRACE_READ(level, prefix)					\
	do {								\
		if (*op == 'R')						\
			trace(level, prefix " %08x %s %08x %x %s\n",	\
			    ioc_pc, op, address, value, rd_reg[address&0xf]);		\
	} while(0)

struct chan {
	struct elastic	*ep;
	uint8_t		mode[2];
	uint8_t		rxhold;
	int		mrptr;
	int		sr;
	int		txon;
	int		rxon;
	int		isdiag;
};

static struct ioc_duart {
	uint8_t		wrregs[16];
	uint8_t		rdregs[16];
	struct chan	chan[2];
	uint8_t		opr;
} ioc_duart[1];

/**********************************************************************/

unsigned int v_matchproto_(iofunc_f)
io_duart(
    const char *op,
    unsigned int address,
    memfunc_f *func,
    unsigned int value
)
{
	struct chan *chp;
	int8_t chr;
	// uint8_t chr;

	(void)func;
	DUART_TRACE_WRITE(2, "DUART");

	if (op[0] == 'W')
		(void)func(op, ioc_duart->wrregs, address & 0x0f, value);
	else
		value = func(op, ioc_duart->rdregs, address & 0x0f, value);

	chp = &ioc_duart->chan[(address>>3)&1];

	switch (address & 0xf) {
	case 0x0:	// MR1A/MR2A
	case 0x8:	// MR1B/MR2B
		if (op[0] == 'W') {
			chp->mode[chp->mrptr] = value;
			chp->mrptr ^= 1;
		}
		break;
	case 0x1:	// R:SRA, W:CSRA
	case 0x9:	// R:SRB, W:CSRB
		if (op[0] == 'R') {
			value = chp->sr;
		}
		break;
	case 0x2:	// CRA
	case 0xa:	// CRB
		if (op[0] == 'W') {
			switch ((value >> 4) & 0x7) {
			case 0x4:
				chp->mrptr = 0;
				break;
			default:
				break;
			}
			if (value & 1) {
				chp->rxon = 1;
			}
			if (value & 2) {
				chp->rxon = 0;
			}
			if (value & 4) {
				chp->txon = 1;
				chp->sr |= 4;
			}
			if (value & 8) {
				chp->txon = 0;
				chp->sr &= ~4;
			}
		}
		break;
	case 0x3:	// THRA
	case 0xb:	// THRB
		if (op[0] == 'W') {
			chr = value;
			elastic_put(chp->ep, &chr, 1);
			if (chp->isdiag && ioc_duart->opr & 4) {
				// IOP.DLOOP~ on p21
				chp->rxhold = chr;
				chp->sr |= 1;
			} else if (chp->mode[1] & 0x80) {
				// Internal Local Loop
				chp->rxhold = chr;
				chp->sr |= 1;
			}
		} else {
			value = chp->rxhold;
			chp->sr &= ~1;
		}
		break;
	case 0xe:	// Set Output Port Bits Command
		ioc_duart->opr |= value;
		break;
	case 0xf:	// Set Output Port Bits Command
		ioc_duart->opr &= ~value;
		break;
	default:
		break;
	}
	DUART_TRACE_READ(2, "DUART");

	return (value);
}

void v_matchproto_(cli_func_f)
cli_ioc_duart(struct cli *cli)
{
	struct chan *chp = &ioc_duart->chan[1];

	if (cli->help) {
		cli_io_help(cli, "IOC duart", 0, 1);
		return;
	}

	cli->ac--;
	cli->av++;

	while (cli->ac && !cli->status) {
		if (cli_elastic(chp->ep, cli))
			continue;
		if (cli->ac >= 1 && !strcmp(cli->av[0], "test")) {
			elastic_put(chp->ep, "Hello World\r\n", -1);
			cli->ac -= 1;
			cli->av += 1;
			continue;
		}
		cli_unknown(cli);
		break;
	}
}

void
ioc_duart_init(struct sim *sim)
{

	ioc_duart->chan[0].ep = elastic_new(sim, O_RDWR);
	ioc_duart->chan[1].ep = elastic_new(sim, O_RDWR);
	ioc_duart->chan[1].isdiag = 1;
}
