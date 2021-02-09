/*
 * MODEM/DIAG DUART - SCN2681 Dual asynchronous receiver/transmitter
 * -----------------------------------------------------------------
 *
 * Chip Select: 0xffffaxxx
 *
 */

#include <fcntl.h>
#include <pthread.h>
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

#define REG_W_MRA	0
#define REG_W_CSRA	1
#define REG_W_CRA	2
#define REG_W_THRA	3
#define REG_W_ACR	4
#define REG_W_IMR	5
#define REG_W_CTUR	6
#define REG_W_CTLR	7
#define REG_W_MRB	8
#define REG_W_CSRB	9
#define REG_W_CRB	10
#define REG_W_THRB	11
#define REG_W_OPCR	13
#define REG_W_SET_BITS	14
#define REG_W_CLR_BITS	15

#define REG_R_MRA	0
#define REG_R_SRA	1
#define REG_R_BRG_TEST	2
#define REG_R_RHRA	3
#define REG_R_IPCR	4
#define REG_R_ISR	5
#define REG_R_CTU	6
#define REG_R_CTL	7
#define REG_R_MRB	8
#define REG_R_SRB	9
#define REG_R_1_16_TEST	10
#define REG_R_RHRB	11
#define REG_R_INPUT	13
#define REG_R_START_PIT	14
#define REG_R_STOP_PIT	15

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
	int		pit_running;
	int		pit_intr;
} ioc_duart[1];

static pthread_mutex_t duart_mtx = PTHREAD_MUTEX_INITIALIZER;

/**********************************************************************/

static void
ioc_duart_pit_callback(void *priv)
{
	(void)priv;
	AZ(pthread_mutex_lock(&duart_mtx));
	if (ioc_duart->pit_running) {
		if ((--ioc_duart->rdregs[REG_R_CTL]) == 0xff)
			ioc_duart->rdregs[REG_R_CTU]--;
		if (!ioc_duart->rdregs[REG_R_CTU] &&
		    !ioc_duart->rdregs[REG_R_CTL]) {
			trace(TRACE_PIT, "PIT ZERO\n");
			ioc_duart->rdregs[REG_R_ISR] |= 0x8;
			if (ioc_duart->pit_intr) {
				irq_raise(&IRQ_PIT);
				ioc_duart->opr |= 0x08;
			}
		}
	}
	AZ(pthread_mutex_unlock(&duart_mtx));
}

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
	unsigned reg;
	int8_t chr;
	// uint8_t chr;

	reg = address & 0xf;
	chp = &ioc_duart->chan[reg>>3];

	AZ(pthread_mutex_lock(&duart_mtx));
	if (op[0] == 'W') {
		trace(TRACE_IO, "DUART %08x %s %08x %x %s\n",
		    ioc_pc, op, address, value, wr_reg[reg]);
		(void)func(op, ioc_duart->wrregs, reg, value);
		switch(reg) {
		case REG_W_MRA:
		case REG_W_MRB:
			chp->mode[chp->mrptr] = value;
			chp->mrptr ^= 1;
			break;
		case REG_W_CRA:
		case REG_W_CRB:
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
			break;
		case REG_W_THRA:
		case REG_W_THRB:
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
			break;
		case REG_W_OPCR:
			ioc_duart->pit_intr = (value & 0x0c) == 0x04;
			break;
		case REG_W_SET_BITS:
			ioc_duart->opr |= value;
			break;
		case REG_W_CLR_BITS:
			ioc_duart->opr &= ~value;
			break;
		default:
			break;
		}
	} else {
		value = func(op, ioc_duart->rdregs, reg, value);
		switch(reg) {
		case REG_R_SRA:
		case REG_R_SRB:
			value = chp->sr;
			break;
		case REG_R_RHRA:
		case REG_R_RHRB:
			value = chp->rxhold;
			chp->sr &= ~1;
			break;
		case REG_R_START_PIT:
			ioc_duart->rdregs[REG_R_CTU] =
			    ioc_duart->wrregs[REG_W_CTUR];
			ioc_duart->rdregs[REG_R_CTL] =
			    ioc_duart->wrregs[REG_W_CTLR];
			ioc_duart->pit_running = 1;
			trace(TRACE_PIT, "PIT START 0x%02x%02x\n",
			    ioc_duart->rdregs[REG_R_CTU],
			    ioc_duart->rdregs[REG_R_CTL]);
			break;
		case REG_R_STOP_PIT:
			trace(TRACE_PIT, "PIT STOP\n");
			ioc_duart->pit_running = 0;
			ioc_duart->rdregs[REG_R_ISR] &= ~0x8;
			irq_lower(&IRQ_PIT);
			if (ioc_duart->pit_intr)
				ioc_duart->opr &= ~0x08;
			break;
		default:
			break;
		}
		trace(TRACE_IO, "DUART %08x %s %08x %x %s\n",
		    ioc_pc, op, address, value, rd_reg[reg]);
	}

	AZ(pthread_mutex_unlock(&duart_mtx));
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
	/*
	 * The IOC_EEPROM1 selftest seems to assume that CTLR is
	 * non-zero after reset (or that the START_COUNTER command
	 * takes some time to kick in ?)
	 *
	 * 80000bae  MOVE.B  IO_DUART_CLR_OPC_STOP_COUNTER,D0
	 * 80000bb2  MOVE.W  #0x0010,IO_DUART_CTUR_CTU
	 * 80000bb8  MOVE.B  IO_DUART_SET_OPC_START_COUNTER,D0
	 * 80000bbc  MOVE.W  IO_DUART_CTUR_CTU,D0
	 * 80000bc0  CMPI.W  #0x0010,D0
	 * 80000bc4  BNE     _TEST_FAILED
	 */
	ioc_duart->wrregs[REG_W_CTLR] = 1;

	ioc_duart->chan[0].ep = elastic_new(sim, O_RDWR);
	ioc_duart->chan[1].ep = elastic_new(sim, O_RDWR);
	ioc_duart->chan[1].isdiag = 1;
	callout_callback(r1000sim, ioc_duart_pit_callback, NULL, 0, 25600);
}
