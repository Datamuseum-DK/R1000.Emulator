#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "vend.h"

#include "r1000.h"
#include "ioc.h"
#include "ioc_scsi.h"

/**********************************************************************/

#ifndef MAP_NOSYNC
#    define MAP_NOSYNC 0
#endif

static int
cli_scsi_dev_map_file(struct cli *cli, struct scsi_dev *dev, const char *fn)
{
	struct stat st[1];
	void *ptr;

	AN(cli);
	AN(dev);
	AN(fn);
	// XXX: cleanup if changing
	dev->fd = open(fn, O_RDONLY);
	if (dev->fd < 0) {
		cli_error(cli, "Cannot open %s: (%s)\n", fn, strerror(errno));
		return (-1);
	}
	AZ(fstat(dev->fd, st));
	if (!S_ISREG(st->st_mode)) {
		cli_error(cli, "Not a regular file: %s\n", fn);
		AZ(close(dev->fd));
		dev->fd = -1;
		return (-1);
	}
	dev->map_size = st->st_size;
	ptr = mmap(
	    NULL,
	    dev->map_size,
	    PROT_READ|PROT_WRITE,
	    MAP_PRIVATE | MAP_NOSYNC,
	    dev->fd,
	    0
	);
	if (ptr == MAP_FAILED) {
		cli_error(cli,
		    "Could not mmap(2): %s (%s)\n", fn, strerror(errno));
		AZ(close(dev->fd));
		dev->fd = -1;
		return (-1);
	}
	dev->map = ptr;
	return (0);
}

/**********************************************************************/

static int v_matchproto_(scsi_func_f)
scsi_00_test_unit_ready(struct scsi_dev *dev, uint8_t *cdb)
{

	(void)cdb;
	trace_scsi_dev(dev, "TEST_UNIT_READY");
	return (IOC_SCSI_OK);
}

static int v_matchproto_(scsi_func_f)
scsi_01_rewind(struct scsi_dev *dev, uint8_t *cdb)
{

	(void)cdb;
	trace_scsi_dev(dev, "REWIND");
	dev->tape_head = 0;
	dev->tape_recno = 0;
	return (IOC_SCSI_OK);
}

static int v_matchproto_(scsi_func_f)
scsi_03_request_sense(struct scsi_dev *dev, uint8_t *cdb)
{
	static uint8_t buf[0x1a];

	trace_scsi_dev(dev, "REQUEST_SENSE");
	buf[0] = 0x80;
	buf[7] = 0x12;
	assert(cdb[4] >= sizeof buf);
	scsi_fm_target(dev, buf, sizeof buf);
	// dev->ctl->regs[0xf] = 2;
	return (IOC_SCSI_OK);
}

static int v_matchproto_(scsi_func_f)
scsi_08_read_6_disk(struct scsi_dev *dev, uint8_t *cdb)
{
	size_t lba;
	size_t nsect;

	trace_scsi_dev(dev, "READ_6(DISK)");

	lba = vbe32dec(cdb) & 0x1fffff;
	nsect = cdb[0x04];

	trace_dump(TRACE_SCSI_DATA, dev->map + (lba<<10), nsect<<10,
	    "READ LBA = 0x%zx * 0x%zx\n", lba, nsect);
	scsi_fm_target(dev, dev->map + (lba<<10), nsect<<10);
	trace(TRACE_SCSI, "SCSI_D READ6 %zx (%08zx)\n", lba, lba << 10);
	return (IOC_SCSI_OK);
}

static int v_matchproto_(scsi_func_f)
scsi_08_read_6_tape(struct scsi_dev *dev, uint8_t *cdb)
{
	unsigned tape_length, xfer_length;

	trace_scsi_dev(dev, "READ_6(TAPE)");

	if (!(dev->ctl->regs[0x12] + dev->ctl->regs[0x13] + dev->ctl->regs[0x14]))
		return (IOC_SCSI_OK);
	xfer_length = vbe32dec(cdb + 1) & 0xffffff;
	tape_length = vle32dec(dev->map + dev->tape_head);
	assert(0 < tape_length);
	assert(tape_length < 65535);
	assert(tape_length <= xfer_length);
	dev->tape_head += 4;

	scsi_fm_target(dev, dev->map + dev->tape_head, tape_length);
	dev->tape_head += tape_length;
	assert(tape_length == vle32dec(dev->map + dev->tape_head));
	dev->tape_head += 4;

	trace(TRACE_SCSI, "SCSI_T READ6 recno=%x head=%zx tape=%x xfer=%x\n",
	    dev->tape_recno, dev->tape_head, tape_length, xfer_length);

	dev->tape_recno++;
	if (tape_length < xfer_length)
		return (xfer_length - tape_length);

	return (IOC_SCSI_OK);
}

static int v_matchproto_(scsi_func_f)
scsi_0a_write_6(struct scsi_dev *dev, uint8_t *cdb)
{
	size_t lba;
	size_t nsect;

	trace_scsi_dev(dev, "WRITE_6");

	lba = vbe32dec(cdb) & 0x1fffff;
	nsect = cdb[0x04];

	scsi_to_target(dev, dev->map + (lba<<10), nsect<<10);
	trace_dump(TRACE_SCSI_DATA, dev->map + (lba<<10), nsect<<10,
	    "WRITE LBA = 0x%zx * 0x%zx\n", lba, nsect);
	trace(TRACE_SCSI, "SCSI_D WRITE6 %zx\n", lba);
	return (IOC_SCSI_OK);
}

static int v_matchproto_(scsi_func_f)
scsi_0b_seek(struct scsi_dev *dev, uint8_t *cdb)
{

	(void)cdb;
	trace_scsi_dev(dev, "SEEK");

	return (IOC_SCSI_OK);
}

static int v_matchproto_(scsi_func_f)
scsi_0d_vendor(struct scsi_dev *dev, uint8_t *cdb)
{

	(void)cdb;
	trace_scsi_dev(dev, "VENDOR");
	return (IOC_SCSI_OK);
}

static int v_matchproto_(scsi_func_f)
scsi_1a_sense(struct scsi_dev *dev, uint8_t *cdb)
{

	trace_scsi_dev(dev, "MODE SENSE");

	switch(cdb[0x02]) {
	case 0x03:
		scsi_fm_target(dev, dev->sense_3, sizeof dev->sense_3);
		break;
	case 0x04:
		scsi_fm_target(dev, dev->sense_4, sizeof dev->sense_4);
		break;
	default:
		trace(TRACE_SCSI, "MODE_SENSE page 0x%d unknown\n", cdb[0x02]);
		return (IOC_SCSI_ERROR);
	}
	return (IOC_SCSI_OK);
}

static int v_matchproto_(scsi_func_f)
scsi_28_read_10(struct scsi_dev *dev, uint8_t *cdb)
{
	size_t lba;
	unsigned nsect;

	trace_scsi_dev(dev, "READ_10");

	lba = vbe32dec(cdb + 0x02);

	nsect = cdb[0x07] << 8;
	nsect |= cdb[0x08];

	scsi_fm_target(dev, dev->map + (lba<<10), nsect<<10);
	trace(TRACE_SCSI, "SCSI_D READ10 %zx\n", lba);
	return (IOC_SCSI_OK);
}

/**********************************************************************/

static scsi_func_f * const scsi_disk_funcs[256] = {
	[0x00] = scsi_00_test_unit_ready,
	[0x08] = scsi_08_read_6_disk,
	[0x0a] = scsi_0a_write_6,
	[0x0b] = scsi_0b_seek,
	[0x0d] = scsi_0d_vendor,
	[0x1a] = scsi_1a_sense,
	[0x28] = scsi_28_read_10,
};

/**********************************************************************/

void v_matchproto_(cli_func_f)
cli_scsi_disk(struct cli *cli)
{
	struct scsi_dev *sd;
	unsigned unit;

	if (cli->help) {
		cli_io_help(cli, "scsi_disk 0 filename", 0, 0);
		return;
	}

	cli->ac--;
	cli->av++;

	if (cli_n_args(cli, 1))
		return;

	unit = atoi(cli->av[0]);
	cli->ac--;
	cli->av++;
	if (unit > 3) {
		cli_error(cli, "Only disks 0-3 supported\n");
		return;
	}

	if (cli_n_args(cli, 0))
		return;

	sd = scsi_d->dev[unit];
	if (sd == NULL) {
		sd = calloc(1, sizeof *sd);
		AN(sd);
		sd->ctl = scsi_d;
		sd->funcs = scsi_disk_funcs;
		scsi_d->dev[unit] = sd;
	}

	if (cli_scsi_dev_map_file(cli, sd, cli->av[0]) < 0)
		return;

	vbe16enc(sd->sense_3 + 0x0c, 0x8316);
	vbe16enc(sd->sense_4 + 0x0c, 0x8412);

	if (sd->map_size == 1143936000UL) {
		// FUJITSU M2266
		vbe16enc(sd->sense_3 + 0x0e, 15);	// tracks/zone
		vbe16enc(sd->sense_3 + 0x14, 45);	// alt sec/lu
		vbe16enc(sd->sense_3 + 0x16, 45);	// sec/track
		vbe16enc(sd->sense_3 + 0x18, 1 << 10);	// sector size
		vbe16enc(sd->sense_3 + 0x1a, 1);	// interleave
		vbe16enc(sd->sense_3 + 0x1c, 5);	// track skew
		vbe16enc(sd->sense_3 + 0x1e, 11);	// cyl skew
		sd->sense_3[0x20] = 0x40;		// flags: HSEC

		vbe16enc(sd->sense_4 + 0x0f, 1658);	// cyl
		sd->sense_4[0x11] = 0x0f;		// nheads
	} else if (sd->map_size == 1115258880UL) {
		// SEAGATE ST41200 "WREN VII"
		vbe16enc(sd->sense_3 + 0x0e, 15);	// tracks/zone
		vbe16enc(sd->sense_3 + 0x14, 45);	// alt sec/lu
		vbe16enc(sd->sense_3 + 0x16, 38);	// sec/track
		vbe16enc(sd->sense_3 + 0x18, 1 << 10);	// sector size
		vbe16enc(sd->sense_3 + 0x1a, 1);	// interleave
		vbe16enc(sd->sense_3 + 0x1c, 3);	// track skew
		vbe16enc(sd->sense_3 + 0x1e, 12);	// cyl skew
		sd->sense_3[0x20] = 0x40;		// flags: HSEC

		vbe16enc(sd->sense_4 + 0x0f, 1931);	// cyl
		sd->sense_4[0x11] = 0x0f;		// nheads
	} else {
		cli_error(cli, "Unknown disk geometry\n");
		return;
	}

	cli->ac--;
	cli->av++;
}

/**********************************************************************/

static scsi_func_f * const scsi_tape_funcs[256] = {
	[0x00] = scsi_00_test_unit_ready,
	[0x01] = scsi_01_rewind,
	[0x03] = scsi_03_request_sense,
	[0x08] = scsi_08_read_6_tape,
};

/**********************************************************************/

void v_matchproto_(cli_func_f)
cli_scsi_tape(struct cli *cli)
{
	struct scsi_dev *sd;

	if (cli->help) {
		cli_io_help(cli, "scsi_tape [filename]", 0, 0);
		return;
	}

	cli->ac--;
	cli->av++;

	if (cli->ac == 0)
		return;

	sd = scsi_t->dev[0];
	if (sd == NULL) {
		sd = calloc(1, sizeof *sd);
		AN(sd);
		sd->ctl = scsi_t;
		sd->funcs = scsi_tape_funcs;
		sd->is_tape = 1;
		scsi_t->dev[0] = sd;
	}

	if (cli_scsi_dev_map_file(cli, sd, cli->av[0]) < 0)
		return;

	cli->ac--;
	cli->av++;
}
