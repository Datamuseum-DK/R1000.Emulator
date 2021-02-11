#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "r1000.h"
#include "ioc.h"
#include "vqueue.h"

struct scsi {
	const char		*name;
	struct irq_vector	*irq_vector;
	pthread_mutex_t		mtx;
	pthread_cond_t		cond;
	pthread_t		thr;
	uint8_t			regs[32];
	unsigned int		dma;
	unsigned		valid_ids;
};

typedef void scsi_func_f(struct scsi *sp);

static char *disk0_image_filename;
static uint8_t sector[1<<10];
static int disk_fd = -1;

struct written {
	VTAILQ_ENTRY(written)	list;
	char			sector[1<<10];
	unsigned int		lba;
};

static VTAILQ_HEAD(,written)	wrote = VTAILQ_HEAD_INITIALIZER(wrote);

/**********************************************************************/

void v_matchproto_(cli_func_f)
cli_scsi_disk(struct cli *cli)
{

	if (cli->help) {
		cli_io_help(cli, "scsi_disk disk0 filename", 0, 1);
		return;
	}

	cli->ac--;
	cli->av++;

	while (cli->ac && !cli->status) {
		if (cli->ac == 2 && !strcmp(cli->av[0], "disk0")) {
			free(disk0_image_filename);
			disk0_image_filename = strdup(cli->av[1]);
			AN(disk0_image_filename);
			cli->ac -= 2;
			cli->av += 2;
			continue;
		}
		cli_unknown(cli);
		break;
	}
}

/**********************************************************************/

static void
get_sector(unsigned secno)
{
	off_t off;
	ssize_t sz;
	struct written *wp;

	if (disk_fd < 0) {
		disk_fd = open(disk0_image_filename, O_RDONLY);
		assert(disk_fd > 0);
	}
	VTAILQ_FOREACH(wp, &wrote, list) {
		if (wp->lba == secno) {
			memcpy(sector, wp->sector, sizeof sector);
			return;
		}
	}
	off = (off_t)secno << 10;
	assert(lseek(disk_fd, off, 0) == off);
	sz = read(disk_fd, sector, sizeof sector);
	assert(sz >= 0);
	assert((size_t)sz == sizeof sector);
}

static void
put_sector(unsigned secno)
{
	struct written *wp;

	VTAILQ_FOREACH(wp, &wrote, list)
		if (wp->lba == secno)
			break;
	if (wp == NULL) {
		wp = malloc(sizeof *wp);
		AN(wp);
		wp->lba = secno;
		VTAILQ_INSERT_HEAD(&wrote, wp, list);
	}
	memcpy(wp->sector, sector, sizeof wp->sector);
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
	unsigned lba, dst, nsect, n;

	trace_cdb(sp, "READ_6");

	lba = sp->regs[0x04] << 16;
	lba |= sp->regs[0x05] << 8;
	lba |= sp->regs[0x06];
	lba &= 0x1fffff;
	nsect = sp->regs[0x07];

	dst = sp->dma;
	dst &= (1<<19)-1;
	for (n = 0; n < nsect; n++) {
		get_sector(lba);
		dma_write(3, dst, sector, sizeof sector);
		lba += 1;
		dst += 1<<10;
	}
	sp->regs[0x17] = 0x16;
	trace(TRACE_SCSI, "SCSI_D READ6 %x -> %x\n", lba, dst);
}

static void v_matchproto_(scsi_func_f)
scsi_0a_write_6(struct scsi *sp)
{
	unsigned lba, dst, nsect, n;

	trace_cdb(sp, "WRITE_6");

	lba = sp->regs[0x04] << 16;
	lba |= sp->regs[0x05] << 8;
	lba |= sp->regs[0x06];
	lba &= 0x1fffff;
	nsect = sp->regs[0x07];

	dst = sp->dma;
	dst &= (1<<19)-1;
        for (n = 0; n < nsect; n++) {
		dma_read(3, dst, sector, sizeof sector);
		put_sector(lba);
		dst += 1<<10;
		lba += 1;
	}
	sp->regs[0x17] = 0x16;
	trace(TRACE_SCSI, "SCSI_D WRITE6 %x -> %x\n", lba, dst);
}

static void v_matchproto_(scsi_func_f)
scsi_0b_seek(struct scsi *sp)
{

	trace_cdb(sp, "SEEK");

	sp->regs[0x17] = 0x16;
}

static void v_matchproto_(scsi_func_f)
scsi_0d_vendor(struct scsi *sp)
{

	trace_cdb(sp, "VENDOR");

	sp->regs[0x17] = 0x16;
}

#if defined(SEAGATE_WREN)

static uint8_t mode_sense_page_3[] = {
	0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
        0x83,			// cc0c Page Code
	0x16,			// cc0d Page Length
        0x00, 0x0f,		// cc0e Tracks per Zone:  15
	0x00, 0x00,		// cc10 Alternate Sectors per Zone:  0
	0x00, 0x00,		// cc12 Alternate Tracks per Zone:  0
	0x00, 0x2d,		// cc14 Alternate Tracks per Logical Unit:  45
        0x00, 0x26,		// cc16 Sectors per Track:  38
	0x04, 0x00,		// cc18 Data Bytes per Physical Sector:  1024
	0x00, 0x01,		// Interleave:  1
	0x00, 0x03,		// Track Skew Factor:  3
        0x00, 0x0c,		// Cylinder Skew Factor:  12
	0x40, 			// SSEC:  0, HSEC:  1, RMB:  0, SURF:  0
	0x00, 0x00, 0x00	// Reserved

};

static uint8_t mode_sense_page_4[] = {
	0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
        0x84,			// cc0c Page Code
	0x12,			// cc0d Page Length
        0x00, 0x07, 0x8b,	// cc0e Number of Cylinders:  1931
	0x0f,			// cc11 Number of Heads:  15
	0x00, 0x00, 0x00,	// Starting Cylinder-Write Precompensation:  0
	0x00, 0x00, 0x00,	// Starting Cylinder-Reduced Write Current:  0
	0x00, 0x00,		// Drive Step Rate:  0
	0x00, 0x00, 0x00,	// Landing Zone Cylinder:  0
	0x00,			// RPL:  0
        0x00,			// Rotational Offset:  0
	0x00			// Reserved
};

#else

static uint8_t mode_sense_page_3[] = {
	0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
        0x83,			// cc0c Page Code
	0x16,			// cc0d Page Length
        0x00, 0x0f,		// cc0e Tracks per Zone:  15
	0x00, 0x00,		// cc10 Alternate Sectors per Zone:  0
	0x00, 0x00,		// cc12 Alternate Tracks per Zone:  0
	0x00, 0x2d,		// cc14 Alternate Tracks per Logical Unit:  45
        0x00, 0x2d,		// cc16 Sectors per Track:  38
	0x04, 0x00,		// cc18 Data Bytes per Physical Sector:  1024
	0x00, 0x01,		// Interleave:  1
	0x00, 0x05,		// Track Skew Factor:  3
        0x00, 0x0b,		// Cylinder Skew Factor:  12
	0x40, 			// SSEC:  0, HSEC:  1, RMB:  0, SURF:  0
	0x00, 0x00, 0x00	// Reserved

};

static uint8_t mode_sense_page_4[] = {
	0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
        0x84,			// cc0c Page Code
	0x12,			// cc0d Page Length
        0x00, 0x06, 0x7a,	// cc0e Number of Cylinders:  1931
	0x0f,			// cc11 Number of Heads:  15
	0x00, 0x00, 0x00,	// Starting Cylinder-Write Precompensation:  0
	0x00, 0x00, 0x00,	// Starting Cylinder-Reduced Write Current:  0
	0x00, 0x00,		// Drive Step Rate:  0
	0x00, 0x00, 0x00,	// Landing Zone Cylinder:  0
	0x00,			// RPL:  0
        0x00,			// Rotational Offset:  0
	0x00			// Reserved
};


#endif

static void v_matchproto_(scsi_func_f)
scsi_1a_mode_sense(struct scsi *sp)
{
	unsigned dst;

	trace_cdb(sp, "MODE SENSE");

	dst = sp->dma;
	dst &= (1<<19)-1;
	switch(sp->regs[0x05]) {
	case 0x03:
		dma_write(3, dst, mode_sense_page_3, sizeof mode_sense_page_3);
		break;
	case 0x04:
		dma_write(3, dst, mode_sense_page_4, sizeof mode_sense_page_4);
		break;
	}
	sp->regs[0x17] = 0x16;
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
	trace(TRACE_SCSI, "SCSI_D READ10 %x -> %x\n", lba, dst);
}

static scsi_func_f * const scsi_funcs[256] = {
	[0x00] = scsi_00_test_unit_ready,
	[0x08] = scsi_08_read_6,
	[0x0a] = scsi_0a_write_6,
	[0x0b] = scsi_0b_seek,
	[0x0d] = scsi_0d_vendor,
	[0x1a] = scsi_1a_mode_sense,
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
			irq_raise(sp->irq_vector);
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
			irq_raise(sp->irq_vector);
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
		irq_raise(sp->irq_vector);
	}
}

static unsigned
scsi_ctrl(struct scsi *sp, const char *op, unsigned int address, memfunc_f *func, unsigned int value)
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

	AZ(pthread_mutex_lock(&sp->mtx));
	sp->regs[0x1f] |= 0x80;
	sp->regs[0x17] = 0x00;
	irq_raise(sp->irq_vector);
	AZ(pthread_mutex_unlock(&sp->mtx));
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

/**********************************************************************/

static uint8_t ctl_regs[512];

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
	unsigned prev_t = ctl_regs[3] & 0x10;

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
			callout_callback(r1000sim, scsi_ctrl_reset, sp, 5000, 0);
		}
		if (prev_t && !(ctl_regs[3] & 0x10)) {
			sp = scsi_t;
			trace(2, "SCSI_CTL SCSI_T RESET\n");
			irq_lower(sp->irq_vector);
			callout_callback(r1000sim, scsi_ctrl_reset, sp, 5000, 0);
		}
	} else {
		ctl_regs[1] |= 0x80;	// WRITE ENABLE SWITCH
		value = func(op, ctl_regs, address & 0x1ff, value);
		IO_TRACE_READ(2, "SCSI_CTL");
	}
	return (value);
}
