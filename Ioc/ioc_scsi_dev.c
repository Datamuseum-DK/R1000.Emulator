#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "vend.h"

#include "r1000.h"
#include "ioc.h"
#include "ioc_scsi.h"

/**********************************************************************/

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
scsi_00_test_unit_ready(struct scsi_dev *dev, uint8_t *cdb, unsigned dst)
{

	(void)cdb;
	(void)dst;
	trace_scsi_dev(dev, "TEST_UNIT_READY");
	return (IOC_SCSI_OK);
}

static int v_matchproto_(scsi_func_f)
scsi_08_read_6(struct scsi_dev *dev, uint8_t *cdb, unsigned dst)
{
	size_t lba;
	unsigned nsect;

	trace_scsi_dev(dev, "READ_6");

	lba = cdb[0x01] << 16;
	lba |= cdb[0x02] << 8;
	lba |= cdb[0x03];
	lba &= 0x1fffff;
	nsect = cdb[0x04];

	dma_write(3, dst, dev->map + (lba<<10), nsect<<10);
	trace(TRACE_SCSI, "SCSI_D READ6 %zx -> %x\n", lba, dst);
	return (IOC_SCSI_OK);
}

static int v_matchproto_(scsi_func_f)
scsi_0a_write_6(struct scsi_dev *dev, uint8_t *cdb, unsigned dst)
{
	size_t lba;
	unsigned nsect;

	trace_scsi_dev(dev, "WRITE_6");

	lba = cdb[0x01] << 16;
	lba |= cdb[0x02] << 8;
	lba |= cdb[0x03];
	lba &= 0x1fffff;
	nsect = cdb[0x04];

	dma_read(3, dst, dev->map + (lba<<10), nsect<<10);
	trace(TRACE_SCSI, "SCSI_D WRITE6 %zx -> %x\n", lba, dst);
	return (IOC_SCSI_OK);
}

static int v_matchproto_(scsi_func_f)
scsi_0b_seek(struct scsi_dev *dev, uint8_t *cdb, unsigned dst)
{

	(void)cdb;
	(void)dst;
	trace_scsi_dev(dev, "SEEK");

	return (IOC_SCSI_OK);
}

static int v_matchproto_(scsi_func_f)
scsi_0d_vendor(struct scsi_dev *dev, uint8_t *cdb, unsigned dst)
{

	(void)cdb;
	(void)dst;
	trace_scsi_dev(dev, "VENDOR");
	return (IOC_SCSI_OK);
}

static int v_matchproto_(scsi_func_f)
scsi_1a_sense(struct scsi_dev *dev, uint8_t *cdb, unsigned dst)
{

	trace_scsi_dev(dev, "MODE SENSE");

	switch(cdb[0x02]) {
	case 0x03:
		dma_write(3, dst, dev->sense_3, sizeof dev->sense_3);
		break;
	case 0x04:
		dma_write(3, dst, dev->sense_4, sizeof dev->sense_4);
		break;
	default:
		trace(TRACE_SCSI, "MODE_SENSE page 0x%d unknown\n", cdb[0x02]);
		return (IOC_SCSI_ERROR);
	}
	return (IOC_SCSI_OK);
}

static int v_matchproto_(scsi_func_f)
scsi_28_read_10(struct scsi_dev *dev, uint8_t *cdb, unsigned dst)
{
	size_t lba;
	unsigned nsect;

	trace_scsi_dev(dev, "READ_10");

	lba = (unsigned)cdb[0x02] << 24;
	lba |= cdb[0x03] << 16;
	lba |= cdb[0x04] << 8;
	lba |= cdb[0x05];

	nsect = cdb[0x07] << 8;
	nsect |= cdb[0x08];

	dma_write(3, dst, dev->map + (lba<<10), nsect<<10);
	trace(TRACE_SCSI, "SCSI_D READ10 %zx -> %x\n", lba, dst);
	return (IOC_SCSI_OK);
}

/**********************************************************************/

static scsi_func_f * const scsi_disk_funcs[256] = {
	[0x00] = scsi_00_test_unit_ready,
	[0x08] = scsi_08_read_6,
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
		cli_io_help(cli, "scsi_disk 0 filename", 0, 1);
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

