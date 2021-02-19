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
#include "memspace.h"

static const char * const rd_reg[] = {
	"DUART R MR1A_2A",
	"DUART R SRA",
	"DUART R BRG_Test",
	"DUART R RHRA",
	"DUART R IPCR",
	"DUART R ISR",
	"PIT R CTU",
	"PIT R CTL",
	"DUART R MR1B_2B",
	"DUART R SRB",
	"DUART R 1x/16x_Test",
	"DUART R RHRB",
	"DUART R Reserved",
	"DUART R Input_Port",
	"PIT R Start_Counter_Command",
	"PIT R Stop_Counter_Command",
};

static const char * const wr_reg[] = {
	"DUART W MR1A_2A",
	"DUART W CSRA",
	"DUART W CRA",
	"DUART W THRA",
	"DUART W ACR",
	"DUART W IMR",
	"PIT W CTUR",
	"PIT W CTLR",
	"DUART W MR1B_2B",
	"DUART W CSRB",
	"DUART W CRB",
	"DUART W THRB",
	"DUART W Reserved",
	"DUART W OPCR",
	"DUART W Set_Output_Port_Bits_Command",
	"DUART W Reset_Output_Port_Bits_Command",
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
	struct chan	chan[2];
	uint8_t		opr;
	uint16_t	counter;
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
	if (ioc_duart->pit_running == 1) {
		ioc_duart->pit_running = 2;
	} else if (ioc_duart->pit_running == 2) {
		if (!--ioc_duart->counter) {
			trace(TRACE_PIT, "PIT ZERO\n");
			io_duart_rd_space[REG_R_ISR] |= 0x8;
			if (ioc_duart->pit_intr) {
				irq_raise(&IRQ_PIT);
				ioc_duart->opr |= 0x08;
			}
		}
	}
	AZ(pthread_mutex_unlock(&duart_mtx));
}

/**********************************************************************/

void v_matchproto_(mem_pre_read)
io_duart_pre_read(int debug, uint8_t *space, unsigned width, unsigned adr)
{
	struct chan *chp;

	if (debug) return;
	assert (width == 1);
	assert (adr < 16);

	AZ(pthread_mutex_lock(&duart_mtx));
	chp = &ioc_duart->chan[adr>>3];
	switch(adr) {
	case REG_R_SRA:
	case REG_R_SRB:
		space[adr] = chp->sr;
		break;
	case REG_R_RHRA:
	case REG_R_RHRB:
		space[adr] = chp->rxhold;
		chp->sr &= ~1;
		break;
	case REG_R_START_PIT:
		ioc_duart->counter = io_duart_wr_space[REG_W_CTLR];
		ioc_duart->counter |= io_duart_wr_space[REG_W_CTUR] << 8;
		io_duart_rd_space[REG_R_CTU] = ioc_duart->counter >> 8;
		io_duart_rd_space[REG_R_CTL] = ioc_duart->counter;
		ioc_duart->pit_running = 1;
		trace(TRACE_PIT, "PIT START 0x%02x%02x\n",
		    io_duart_rd_space[REG_R_CTU],
		    io_duart_rd_space[REG_R_CTL]);
		break;
	case REG_R_STOP_PIT:
		trace(TRACE_PIT, "PIT STOP\n");
		ioc_duart->pit_running = 0;
		io_duart_rd_space[REG_R_ISR] &= ~0x8;
		irq_lower(&IRQ_PIT);
		if (ioc_duart->pit_intr)
			ioc_duart->opr &= ~0x08;
		break;
	default:
		break;
	}
	AZ(pthread_mutex_unlock(&duart_mtx));
	trace(TRACE_IO, "%s [%x] -> %x\n", rd_reg[adr], adr, space[adr]);
}

/**********************************************************************/

void v_matchproto_(mem_post_write)
io_duart_post_write(int debug, uint8_t *space, unsigned width, unsigned adr)
{
	struct chan *chp;
	int8_t chr;

	if (debug) return;
	assert (width == 1);
	assert (adr < 16);
	trace(TRACE_IO, "%s [%x] <- %x\n", wr_reg[adr], adr, space[adr]);
	AZ(pthread_mutex_lock(&duart_mtx));
	chp = &ioc_duart->chan[adr>>3];
	switch(adr) {
	case REG_W_MRA:
	case REG_W_MRB:
		chp->mode[chp->mrptr] = space[adr];
		chp->mrptr ^= 1;
		break;
	case REG_W_CRA:
	case REG_W_CRB:
		switch ((space[adr] >> 4) & 0x7) {
		case 0x4:
			chp->mrptr = 0;
			break;
		default:
			break;
		}
		if (space[adr] & 1) {
			chp->rxon = 1;
		}
		if (space[adr] & 2) {
			chp->rxon = 0;
		}
		if (space[adr] & 4) {
			chp->txon = 1;
			chp->sr |= 4;
		}
		if (space[adr] & 8) {
			chp->txon = 0;
			chp->sr &= ~4;
		}
		break;
	case REG_W_THRA:
	case REG_W_THRB:
		chr = space[adr];
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
		ioc_duart->pit_intr = (space[adr] & 0x0c) == 0x04;
		break;
	case REG_W_SET_BITS:
		ioc_duart->opr |= space[adr];
		if (ioc_duart->opr & 0x10)
			irq_raise(&IRQ_MODEM_RXRDY);
		if (ioc_duart->opr & 0x20)
			irq_raise(&IRQ_DIAG_BUS_RXRDY);
		if (ioc_duart->opr & 0x80)
			irq_raise(&IRQ_DIAG_BUS_TXRDY);
		break;
	case REG_W_CLR_BITS:
		ioc_duart->opr &= ~space[adr];
		if (!(ioc_duart->opr & 0x10))
			irq_lower(&IRQ_MODEM_RXRDY);
		if (!(ioc_duart->opr & 0x10))
			irq_lower(&IRQ_DIAG_BUS_RXRDY);
		if (!(ioc_duart->opr & 0x80))
			irq_lower(&IRQ_DIAG_BUS_TXRDY);
		break;
	default:
		break;
	}
	AZ(pthread_mutex_unlock(&duart_mtx));
}

/**********************************************************************/

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
	callout_callback(r1000sim, ioc_duart_pit_callback, NULL, 0, 25600);
}
