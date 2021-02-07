#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "r1000.h"
#include "ioc.h"

struct scsi {
	const char		*name;
	struct irq_vector	*irq_vector;
	pthread_mutex_t		mtx;
	pthread_cond_t		cond;
	pthread_t		thr;
	uint8_t			regs[32];
	uint8_t			r[1<<16];
	int			raised;
	unsigned int		dma;
	unsigned		valid_ids;
};

typedef void scsi_func_f(struct scsi *sp);

/**********************************************************************/

static uint8_t sector[1<<10];

static int disk_fd = -1;
// #define DISK_IMAGE "/critter/DDHF/R1000/R1K_Seagate/R1K_Seagate0.BIN"

// This file is a copy of https://datamuseum.dk/bits/30000551
#define DISK_IMAGE "/critter/DDHF/20191107_R1K_TAPES/R1K/PE_R1K_Disk0.dd"

static void
get_sector(unsigned secno)
{
	off_t off;
	ssize_t sz;

	if (disk_fd < 0) {
		disk_fd = open(DISK_IMAGE, O_RDONLY);
		assert(disk_fd > 0);
	}
	off = (off_t)secno << 10;
	assert(lseek(disk_fd, off, 0) == off);
	sz = read(disk_fd, sector, sizeof sector);
	assert(sz >= 0);
	assert((size_t)sz == sizeof sector);
}

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
trace_cdb(struct scsi *sp, const char *cmt)
{

	trace(
	    2,
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

/**********************************************************************/

static void v_matchproto_(scsi_func_f)
scsi_00_test_unit_ready(struct scsi *sp)
{

	trace_cdb(sp, "TEST_UNIT_READY");

	sp->regs[0x17] = 0x16;
}

static void v_matchproto_(scsi_func_f)
scsi_08_read_6(struct scsi *sp)
{
	unsigned lba, dst, nsect;

	trace_cdb(sp, "READ_6");

	lba = sp->regs[0x04] << 16;
	lba |= sp->regs[0x05] << 8;
	lba |= sp->regs[0x06];
	lba &= 0x1fffff;
	nsect = sp->regs[0x07];
	assert(nsect == 1);

	get_sector(lba);
	dst = sp->dma;
	dst &= (1<<19)-1;
	dma_write(3, dst, sector, sizeof sector);
	sp->regs[0x17] = 0x16;
	trace(2, "SCSI_D READ6 %x -> %x\n", lba, dst);
}

static void v_matchproto_(scsi_func_f)
scsi_28_read_10(struct scsi *sp)
{
	unsigned lba, dst, nsect;

	trace_cdb(sp, "READ_10");

	lba = (unsigned)sp->regs[0x05] << 24;
	lba |= sp->regs[0x06] << 16;
	lba |= sp->regs[0x07] << 8;
	lba |= sp->regs[0x08];

	nsect = sp->regs[0x0a] << 8;
	nsect |= sp->regs[0x0b];
	assert(nsect == 1);

	get_sector(lba);
	dst = sp->dma;
	dst &= (1<<19)-1;	// Probably wrong mask.
	dma_write(3, dst, sector, sizeof sector);
	sp->regs[0x17] = 0x16;
	trace(2, "SCSI_D READ10 %x -> %x\n", lba, dst);
}

static scsi_func_f * const scsi_funcs[256] = {
	[0x00] = scsi_00_test_unit_ready,
	[0x08] = scsi_08_read_6,
	[0x28] = scsi_28_read_10,
};

static void *
scsi_thread(void *priv)
{
	struct scsi *sp = priv;
	scsi_func_f *sf;
	unsigned id;

	AZ(pthread_mutex_lock(&sp->mtx));
	while (1) {
		AZ(pthread_cond_wait(&sp->cond, &sp->mtx));

		if (sp->regs[0x18] == 0) {		// RESET
			sp->regs[0x17] = 0x01;
			sp->regs[0x1f] |= 0x80;
			if (!sp->raised)
				sp->raised = irq_raise(sp->irq_vector);
			continue;
		}

		if (sp->regs[0x18] != 8) {
			WRONG();
		}

		id = sp->regs[0x15] & 7;
		if (!(sp->valid_ids & (1U << id))) {
			trace_cdb(sp, "No Device at ID");
			sp->regs[0x17] = 0x42;
			sp->regs[0x1f] |= 0x80;
			if (!sp->raised)
				sp->raised = irq_raise(sp->irq_vector);
			continue;
		}

		sf = scsi_funcs[sp->regs[0x03]];
		if (sf != NULL) {
			sf(sp);
		} else {
			trace_cdb(sp, "UNIMPL");
			sp->regs[0x17] = 0x42;
		}
		sp->regs[0x1f] |= 0x80;
		if (!sp->raised)
			sp->raised = irq_raise(sp->irq_vector);
	}
}

static unsigned
scsi_ctrl(struct scsi *sp, const char *op, unsigned int address, memfunc_f *func, unsigned int value)
{
	unsigned reg;

	reg = address & 0x1f;
	if (*op == 'W')
		trace(2, "%s %08x %s %s %x\n", sp->name,
		    ioc_pc, op, scsi_reg[reg], value);
	AZ(pthread_mutex_lock(&sp->mtx));
	value = func(op, sp->regs, reg, value);
	if (op[0] == 'W' && reg == 0x18) {
		AZ(pthread_cond_signal(&sp->cond));
	}
	if (op[0] == 'R' && reg == 0x1f && (value & 0x80)) {
		sp->regs[0x1f] &= ~0x80;
		if (sp->raised)
			sp->raised = irq_lower(sp->irq_vector);
	}
	AZ(pthread_mutex_unlock(&sp->mtx));
	if (*op == 'R')
		trace(2, "%s %08x %s %s %x\n", sp->name,
		    ioc_pc, op, scsi_reg[reg], value);
	return (value);
}

/**********************************************************************/

static struct scsi scsi_d[1];

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

unsigned int v_matchproto_(iofunc_f)
io_scsi_d(
    const char *op,
    unsigned int address,
    memfunc_f *func,
    unsigned int value
)
{
	struct scsi *sp = scsi_d;

	IO_TRACE_WRITE(2, "SCSI_D");
	if (op[0] == 'W') {
		if (address == 0x9303e100) {
			sp->dma &= 0xffff0000;
			sp->dma |= value;
		}
		if (address == 0x9303e108) {
			sp->dma &= 0xffff;
			sp->dma |= value << 16;
		}
		if (sp->r[0] == 0 && address == 0x9303e000 && value == 1) {
			sp->regs[0x1f] |= 0x80;
			sp->regs[0x17] = 0x01;
		}
	}
	value = func(op, sp->r, address & 0xffff, value);
	IO_TRACE_READ(2, "SCSI_D");
	return (value);
}

void
ioc_scsi_d_init(struct sim *cs)
{
	(void)cs;
	scsi_d->name = "SCSI_D";
	scsi_d->irq_vector = &IRQ_SCSI_D;
	scsi_d->valid_ids = 0x3;
	AZ(pthread_mutex_init(&scsi_d->mtx, NULL));
	AZ(pthread_cond_init(&scsi_d->cond, NULL));
	AZ(pthread_create(&scsi_d->thr, NULL, scsi_thread, scsi_d));
}

/**********************************************************************/

static struct scsi scsi_t[1];

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

unsigned int v_matchproto_(iofunc_f)
io_scsi_t(
    const char *op,
    unsigned int address,
    memfunc_f *func,
    unsigned int value
)
{
	struct scsi *sp = scsi_t;

	IO_TRACE_WRITE(2, "SCSI_T");
	if (op[0] == 'W') {
		if (address == 0x9303e100) {
			sp->dma &= 0xffff0000;
			sp->dma |= value;
		}
		if (address == 0x9303e108) {
			sp->dma &= 0xffff;
			sp->dma |= value << 16;
		}
		if (address == 0x9303e002 && value) {
			sp->regs[0x1f] |= 0x80;
			sp->regs[0x17] = 0x01;
		}
		if (address == 0x9303e008 && value) {
			sp->regs[0x1f] |= 0x80;
			sp->regs[0x17] = 0x01;
		}
	}
	value = func(op, scsi_t->r, address & 0xffff, value);
	IO_TRACE_READ(2, "SCSI_T");
	return (value);
}

void
ioc_scsi_t_init(struct sim *cs)
{
	(void)cs;
	scsi_t->name = "SCSI_T";
	scsi_t->irq_vector = &IRQ_SCSI_T;
	scsi_t->valid_ids = 0x1;
	AZ(pthread_mutex_init(&scsi_t->mtx, NULL));
	AZ(pthread_cond_init(&scsi_t->cond, NULL));
	AZ(pthread_create(&scsi_t->thr, NULL, scsi_thread, scsi_t));
}
