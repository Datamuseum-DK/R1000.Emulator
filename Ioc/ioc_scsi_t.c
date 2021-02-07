#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "r1000.h"
#include "ioc.h"

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

static uint8_t scsi_t[1<<12];

static void
scsi_cmd(void)
{
	// unsigned lba, dst, nsect;

	trace(2, "SCSI_T CMD=%02x ID=%x CDB=[%02x %02x %02x %02x %02x %02x|%02x %02x %02x %02x]\n",
	    scsi_t[0xc18],
	    scsi_t[0xc15] & 7,
	    scsi_t[0xc03],
	    scsi_t[0xc04],
	    scsi_t[0xc05],
	    scsi_t[0xc06],
	    scsi_t[0xc07],
	    scsi_t[0xc08],
	    scsi_t[0xc09],
	    scsi_t[0xc0a],
	    scsi_t[0xc0b],
	    scsi_t[0xc0c]
	);
	if (scsi_t[0xc18] == 0) {
		// RESET
		scsi_t[0xc17] = 0x01;
		return;
	}
	assert (scsi_t[0xc18] == 8);

	if ((scsi_t[0xc15]&7) > 1) {
		// Only ID Zero and One
		scsi_t[0xc17] = 0x42;
		return;
	}

	switch (scsi_t[0xc03]) {
	case 0x00:	// TEST_UNIT_READY
		scsi_t[0xc17] = 0x16;
		return;
#if 0
	case 0x08:	// READ_6
		lba = scsi_t[0xc04] << 16;
		lba |= scsi_t[0xc05] << 8;
		lba |= scsi_t[0xc06];
		lba &= 0x1fffff;
		nsect = scsi_t[0xc07];
		assert(nsect == 1);

		get_sector(lba);
		dst = scsi_t[0xe101];
		dst |= scsi_t[0xe100] << 8;
		dst |= scsi_t[0xe109] << 16;
		dst |= (unsigned)scsi_t[0xe108] << 24;
		dst &= (1<<19)-1;
		dma_write(3, dst, sector, sizeof sector);
		scsi_t[0xc17] = 0x16;
		trace(2, "SCSI_T READ6 %x -> %x\n", lba, dst);
		return;
	case 0x0d:	// Vendor Specific
		scsi_t[0xc17] = 0x42;
		return;
	case 0x28:	// READ_10
		lba = (unsigned)scsi_t[0xc05] << 24;
		lba |= scsi_t[0xc06] << 16;
		lba |= scsi_t[0xc07] << 8;
		lba |= scsi_t[0xc08];

		nsect = scsi_t[0xc0a] << 8;
		nsect |= scsi_t[0xc0b];
		assert(nsect == 1);

		get_sector(lba);
		dst = scsi_t[0xe101];
		dst |= scsi_t[0xe100] << 8;
		dst |= scsi_t[0xe109] << 16;
		dst |= (unsigned)scsi_t[0xe108] << 24;
		dst &= (1<<19)-1;	// Probably wrong mask.
		dma_write(3, dst, sector, sizeof sector);
		scsi_t[0xc17] = 0x16;
		trace(2, "SCSI_T READ10 %x -> %x\n", lba, dst);
		return;
#endif
	default:
		printf("UNKNOWN SCSI_T 0x%02x 0x%02x\n", scsi_t[0xc18], scsi_t[0xc03]);
		scsi_t[0xc17] = 0x42;
		return;
	}
}

unsigned int v_matchproto_(iofunc_f)
io_scsi_t(
    const char *op,
    unsigned int address,
    memfunc_f *func,
    unsigned int value
)
{

	if ((address & ~0x1f) == 0x9303ec00)
		SCSI_TRACE_WRITE(2, "SCSI_T");
	else
		IO_TRACE_WRITE(2, "SCSI_T");
	//if (scsi_t[0x8] == 0 && op[0] == 'W' && address == 0x9303e008 && value == 0x30) {
	if (op[0] == 'W' && address == 0x9303e002 && value) {
		trace(2, "SCSI_T RESET 2 %02x %02x %02x %02x\n", scsi_t[0], scsi_t[1], scsi_t[2], scsi_t[3]);
		scsi_t[0xc1f] |= 0x80;
		scsi_t[0xc17] = 0x01;
	}
	if (op[0] == 'W' && address == 0x9303e008 && value) {
		trace(2, "SCSI_T RESET 8 %02x %02x %02x %02x\n", scsi_t[8], scsi_t[9], scsi_t[10], scsi_t[11]);
		scsi_t[0xc1f] |= 0x80;
		scsi_t[0xc17] = 0x01;
	}
	value = func(op, scsi_t, address & 0xfff, value);
	if (op[0] == 'W' && address == 0x9303ec18) {
		scsi_cmd();
		scsi_t[0xc1f] |= 0x80;
	}
	if (op[0] == 'R' && address == 0x9303ec1f && (value & 0x80)) {
		scsi_t[0xc1f] &= ~0x80;
	}
	if ((address & ~0x1f) == 0x9303ec00)
		SCSI_TRACE_READ(2, "SCSI_T");
	else
		IO_TRACE_READ(2, "SCSI_T");
	return (value);
}
