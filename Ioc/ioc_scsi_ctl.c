#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "r1000.h"
#include "ioc.h"
#include "ioc_scsi.h"

static uint8_t ctl_regs[512];
struct scsi scsi_t[1];
struct scsi scsi_d[1];

/**********************************************************************/

static const char * const scsi_reg[] = {
	"00_OWN_ID_CDB_SIZE",
	"01_CONTROL",
	"02_TIMEOUT_PERIOD",
	"03_CDB1",
	"04_CDB2",
	"05_CDB3",
	"06_CDB4",
	"07_CDB5",
	"08_CDB6",
	"09_CDB7",
	"0a_CDB8",
	"0b_CDB9",
	"0c_CDB10",
	"0d_CDB11",
	"0e_CDB12",
	"0f_LUN",
	"10_CMD_PHASE",
	"11_SYNC_XFER",
	"12_COUNT2",
	"13_COUNT1",
	"14_COUNT0",
	"15_DEST_ID",
	"16_SRC_ID",
	"17_SCSI_STATUS",
	"18_CMD",
	"19_DATA",
	"1a_unknown",
	"1b_unknown",
	"1c_unknown",
	"1d_unknown",
	"1e_unknown",
	"1f_AUX_STATUS",
};

static void
trace_scsi_ctl(struct scsi *sp, const char *cmt)
{

	trace(
	    TRACE_SCSI,
	    "%s CMD=%02x ID=%x"
	    " CDB=[%02x %02x %02x %02x %02x %02x|%02x %02x %02x %02x]"
	    " %s\n",
	    sp->name,
	    sp->regs[0x18],
	    sp->regs[0x15] & 7,
	    sp->regs[0x03],
	    sp->regs[0x04],
	    sp->regs[0x05],
	    sp->regs[0x06],
	    sp->regs[0x07],
	    sp->regs[0x08],
	    sp->regs[0x09],
	    sp->regs[0x0a],
	    sp->regs[0x0b],
	    sp->regs[0x0c],
	    cmt
	);
}

void
trace_scsi_dev(struct scsi_dev *dev, const char *cmt)
{
	trace_scsi_ctl(dev->ctl, cmt);
}

static void *
scsi_thread(void *priv)
{
	struct scsi *sp = priv;
	struct scsi_dev *sd = NULL;
	scsi_func_f *sf;
	unsigned id;
	unsigned dst;
	int i;

	AZ(pthread_mutex_lock(&sp->mtx));
	while (1) {
		AZ(pthread_cond_wait(&sp->cond, &sp->mtx));

		if (sp->regs[0x18] == 0) {		// RESET
			sp->regs[0x17] = 0x01;
			sp->regs[0x1f] |= 0x80;
			irq_raise(sp->irq_vector);
			continue;
		}

		if (sp->regs[0x18] != 8)
			WRONG();

		id = sp->regs[0x15] & 7;
		sd = sp->dev[id];
		if (sd == NULL) {
			trace_scsi_ctl(sp, "No Device at ID");
			sp->regs[0x17] = 0x42;
		} else if (sd->funcs[sp->regs[0x03]] == NULL) {
			trace_scsi_ctl(sp, "UNIMPL");
			sp->regs[0x17] = 0x42;
		} else {
			dst = sp->dma;
			dst &= (1<<19)-1;
			sf = sd->funcs[sp->regs[0x03]];
			AN(sf);
			i = sf(sd, sp->regs+0x03, dst);
			if (i < 0) {
				sp->regs[0x17] = 0x42;
			} else {
				sp->regs[0x14] = i & 0xff;
				sp->regs[0x13] = (i>>8) & 0xff;
				sp->regs[0x12] = (i>>16) & 0xff;
				if (0 && i)
					sp->regs[0x17] = 0x21;
				else
					sp->regs[0x17] = 0x16;
			}
		}
		sp->regs[0x1f] |= 0x80;
		irq_raise(sp->irq_vector);
	}
}

static unsigned
scsi_ctrl(struct scsi *sp, const char *op, unsigned int address,
    memfunc_f *func, unsigned int value)
{
	unsigned reg;

	reg = address & 0x1f;
	if (*op == 'W')
		trace(TRACE_IO, "%s %08x %s %s %x\n", sp->name,
		    ioc_pc, op, scsi_reg[reg], value);
	AZ(pthread_mutex_lock(&sp->mtx));
	value = func(op, sp->regs, reg, value);
	if (op[0] == 'W' && reg == 0x18) {
		AZ(pthread_cond_signal(&sp->cond));
	}
	if (op[0] == 'R' && reg == 0x17) {
		sp->regs[0x1f] &= ~0x80;
		irq_lower(sp->irq_vector);
	}
	AZ(pthread_mutex_unlock(&sp->mtx));
	if (*op == 'R')
		trace(TRACE_IO, "%s %08x %s %s %x\n", sp->name,
		    ioc_pc, op, scsi_reg[reg], value);
	return (value);
}

static void
scsi_ctrl_reset(void *priv)
{
	struct scsi *sp = priv;
	int id;

	AZ(pthread_mutex_lock(&sp->mtx));
	if (1) {
		for (id = 0; id < 7; id++)
			if (sp->dev[id] != NULL) {
				sp->dev[id]->tape_head = 0;
				sp->dev[id]->tape_recno = 0;
			}
	}
	sp->regs[0x1f] |= 0x80;
	sp->regs[0x17] = 0x00;
	irq_raise(sp->irq_vector);
	AZ(pthread_mutex_unlock(&sp->mtx));
}


/**********************************************************************/

unsigned int v_matchproto_(iofunc_f)
io_scsi_d_reg(
    const char *op,
    unsigned int address,
    memfunc_f *func,
    unsigned int value
)
{
	return scsi_ctrl(scsi_d, op, address, func, value);
}

void
ioc_scsi_d_init(struct sim *cs)
{
	(void)cs;
	scsi_d->name = "SCSI_D";
	scsi_d->irq_vector = &IRQ_SCSI_D;
	AZ(pthread_mutex_init(&scsi_d->mtx, NULL));
	AZ(pthread_cond_init(&scsi_d->cond, NULL));
	AZ(pthread_create(&scsi_d->thr, NULL, scsi_thread, scsi_d));
}

/**********************************************************************/

unsigned int v_matchproto_(iofunc_f)
io_scsi_t_reg(
    const char *op,
    unsigned int address,
    memfunc_f *func,
    unsigned int value
)
{
	return scsi_ctrl(scsi_t, op, address, func, value);
}

void
ioc_scsi_t_init(struct sim *cs)
{
	(void)cs;
	scsi_t->name = "SCSI_T";
	scsi_t->irq_vector = &IRQ_SCSI_T;
	AZ(pthread_mutex_init(&scsi_t->mtx, NULL));
	AZ(pthread_cond_init(&scsi_t->cond, NULL));
	AZ(pthread_create(&scsi_t->thr, NULL, scsi_thread, scsi_t));
}

/**********************************************************************/

unsigned int v_matchproto_(iofunc_f)
io_scsi_ctl(
    const char *op,
    unsigned int address,
    memfunc_f *func,
    unsigned int value
)
{
	struct scsi *sp = NULL;
	unsigned prev_d = ctl_regs[1] & 0x01;
	unsigned prev_t = ctl_regs[9] & 0x10;

	if (op[0] == 'W') {
		IO_TRACE_WRITE(2, "SCSI_CTL");
		(void)func(op, ctl_regs, address & 0x1ff, value);
		if (address == 0x9303e100) {
			sp = scsi_d;
			sp->dma &= 0xffff0000;
			sp->dma |= value;
			trace(2, "SCSI_CTL SCSI_D DMA_ADR %08x\n", sp->dma);
		}
		if (address == 0x9303e104) {
			sp = scsi_t;
			sp->dma &= 0xffff0000;
			sp->dma |= value;
			trace(2, "SCSI_CTL SCSI_T DMA_ADR %08x\n", sp->dma);
		}
		if (address == 0x9303e108) {
			sp = scsi_d;
			sp->dma &= 0xffff;
			sp->dma |= value << 16;
			trace(2, "SCSI_CTL SCSI_D DMA_SEG %08x\n", sp->dma);
		}
		if (address == 0x9303e10c) {
			sp = scsi_t;
			sp->dma &= 0xffff;
			sp->dma |= value << 16;
			trace(2, "SCSI_CTL SCSI_T DMA_SEG %08x\n", sp->dma);
		}
		if (prev_d && !(ctl_regs[1] & 0x01)) {
			sp = scsi_d;
			trace(2, "SCSI_CTL SCSI_D RESET\n");
			irq_lower(sp->irq_vector);
			callout_callback(
			    r1000sim, scsi_ctrl_reset, sp, 5000, 0);
		}
		if (prev_t && !(ctl_regs[9] & 0x10)) {
			sp = scsi_t;
			trace(2, "SCSI_CTL SCSI_T RESET\n");
			irq_lower(sp->irq_vector);
			callout_callback(
			    r1000sim, scsi_ctrl_reset, sp, 5000, 0);
		}
	} else {
		ctl_regs[1] |= 0x80;	// WRITE ENABLE SWITCH

		// This is not on RESHA schematics, but
		//    "RESHA TAPE SCSI sub-tests ..."
		// fails at 0x000713a8 without it
		ctl_regs[1] &= 0x8f;
		ctl_regs[1] |= ctl_regs[9] & 0x70;

		value = func(op, ctl_regs, address & 0x1ff, value);
		IO_TRACE_READ(2, "SCSI_CTL");
	}
	return (value);
}
