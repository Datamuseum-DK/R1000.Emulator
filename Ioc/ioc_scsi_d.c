#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/endian.h>
#include <time.h>
#include <unistd.h>
#include "r1000.h"
#include "ioc.h"
#include "m68k.h"

static uint8_t sector[1<<10];

static void
get_sector(unsigned secno)
{
	static int fd;

	if (!fd) {
		fd = open("/critter/DDHF/R1000/R1K_Seagate/R1K_Seagate0.BIN", O_RDONLY);
		assert(fd > 0);
	}
	lseek(fd, secno << 10, 0);
	read(fd, sector, sizeof sector);
}

static const char *scsi_reg[] = {
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

#define SCSI_TRACE_WRITE(level, prefix)					\
	do {								\
		if (*op == 'W')						\
			trace(level, prefix " %08x %s %s %x\n",		\
			    ioc_pc, op, scsi_reg[address & 0x1f], value);		\
	} while(0)

#define SCSI_TRACE_READ(level, prefix)					\
	do {								\
		if (*op == 'R')						\
			trace(level, prefix " %08x %s %s %x\n",		\
			    ioc_pc, op, scsi_reg[address & 0x1f], value);		\
	} while(0)

static uint8_t scsi_d[1<<16];

static void
scsi_cmd(void)
{
	unsigned lba, dst, nsect;

	trace(2, "SCSI_D CMD=%02x ID=%x CDB=[%02x %02x %02x %02x %02x %02x|%02x %02x %02x %02x]\n",
	    scsi_d[0xe818],
	    scsi_d[0xe815] & 7,
	    scsi_d[0xe803],
	    scsi_d[0xe804],
	    scsi_d[0xe805],
	    scsi_d[0xe806],
	    scsi_d[0xe807],
	    scsi_d[0xe808],
	    scsi_d[0xe809],
	    scsi_d[0xe80a],
	    scsi_d[0xe80b],
	    scsi_d[0xe80c]
	);
	if (scsi_d[0xe818] == 0) {
		// RESET
		scsi_d[0xe817] = 0x01;
		return;
	}
	assert (scsi_d[0xe818] == 8);

	if ((scsi_d[0xe815]&7) > 1) {
		// Only ID Zero and One
		scsi_d[0xe817] = 0x42;
		return;
	}

	switch (scsi_d[0xe803]) {
	case 0x00:	// TEST_UNIT_READY
		scsi_d[0xe817] = 0x16;
		return;
	case 0x08:	// READ_6
		lba = scsi_d[0xe804] << 16;
		lba |= scsi_d[0xe805] << 8;
		lba |= scsi_d[0xe806];
		lba &= 0x1fffff;
		nsect = scsi_d[0xe807];
		assert(nsect == 1);

		get_sector(lba);
		dst = scsi_d[0xe101];
		dst |= scsi_d[0xe100] << 8;
		dst |= scsi_d[0xe109] << 16;
		dst |= scsi_d[0xe108] << 24;
		dst &= (1<<19)-1;
		dma_write(dst, sector, sizeof sector);
		scsi_d[0xe817] = 0x16;
		trace(2, "SCSI_D READ6 %x -> %x\n", lba, dst);
		printf("SCSI_D READ6 %x -> %x\n", lba, dst);
		return;
	case 0x28:	// READ_10
		lba = scsi_d[0xe805] << 24;
		lba |= scsi_d[0xe806] << 16;
		lba |= scsi_d[0xe807] << 8;
		lba |= scsi_d[0xe808];

		nsect = scsi_d[0xe80a] << 8;
		nsect |= scsi_d[0xe80b];
		assert(nsect == 1);

		get_sector(lba);
		dst = scsi_d[0xe101];
		dst |= scsi_d[0xe100] << 8;
		dst |= scsi_d[0xe109] << 16;
		dst |= scsi_d[0xe108] << 24;
// printf("DMA %02x %02x %02x %02x %x\n", scsi_d[0xe108], scsi_d[0xe109], scsi_d[0xe100], scsi_d[0xe101], dst);
		dst &= (1<<19)-1;
		dma_write(dst, sector, sizeof sector);
		scsi_d[0xe817] = 0x16;
		trace(2, "SCSI_D READ10 %x -> %x\n", lba, dst);
		printf("SCSI_D READ10 %x -> %x\n", lba, dst);
		return;
	default:
		printf("UNKNOWN SCSI 0x%02x 0x%02x\n", scsi_d[0xe818], scsi_d[0xe803]);
		scsi_d[0xe817] = 0x00;
		return;
	}
}

unsigned int v_matchproto_(iofunc_f)
io_scsi_d(
    const char *op,
    unsigned int address,
    memfunc_f *func,
    unsigned int value
)
{

	if ((address & ~0x1f) == 0x9303e800)
		SCSI_TRACE_WRITE(2, "SCSI_D");
	else
		IO_TRACE_WRITE(2, "SCSI_D");
	if (scsi_d[0] == 0 && op[0] == 'W' && address == 0x9303e000 && value == 1) {
		scsi_d[0xe81f] |= 0x80;
		scsi_d[0xe817] = 0x01;
	}
	value = func(op, scsi_d, address & 0xffff, value);
	if (op[0] == 'W' && address == 0x9303e818) {
		scsi_cmd();
		scsi_d[0xe81f] |= 0x80;
	}
	if (op[0] == 'R' && address == 0x9303e81f && (value & 0x80)) {
		scsi_d[0xe81f] &= ~0x80;
	}
	if ((address & ~0x1f) == 0x9303e800)
		SCSI_TRACE_READ(2, "SCSI_D");
	else
		IO_TRACE_READ(2, "SCSI_D");
	return (value);
}
