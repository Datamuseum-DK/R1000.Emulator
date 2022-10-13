
//#include <assert.h>
#include <ctype.h>
//#include <fcntl.h>
//#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>

#include "Infra/r1000.h"
#include "Chassis/r1000sc.h"
#include "Diag/diag.h"
#include "Infra/context.h"
//#include "Diag/i8052_emul.h"
//#include "Infra/elastic.h"
//#include "Infra/vsb.h"
//#include "Diag/diag.h"

typedef int board_func_t(struct diagproc_exp_priv *dep, uint8_t length);

static board_func_t fiu_board;
static board_func_t ioc_board;
static board_func_t mem0_board;
static board_func_t seq_board;
static board_func_t typ_board;
static board_func_t val_board;

struct diagproc_exp_priv {
	const char	*name;
	unsigned	state;
	unsigned	counter;
	uint8_t		*ram;
	uint8_t		*ip;
	uint8_t		*dst1;
	uint8_t		*dst2;
	board_func_t	*board;
};

#define SET_PT(dep)	do { *((dep)->ip) |= 2; } while (0)
#define SET_PX(dep)	do { *((dep)->ip) |= 1; } while (0)
#define CLR_PT(dep)	do { *((dep)->ip) &= ~2; } while (0)
#define CLR_PX(dep)	do { *((dep)->ip) &= ~1; } while (0)

void
diagproc_exp_init(struct diagproc_exp_priv **depp, const char *name)
{
	struct diagproc_exp_priv *dep;

	dep = calloc(sizeof *dep, 1);
	AN(dep);
	dep->name = name;
	*depp = dep;
	if (strstr(name, "FIU.fiu") != NULL) {
		dep->board = fiu_board;
	} else if (strstr(name, "IOC.ioc") != NULL) {
		dep->board = ioc_board;
	} else if (strstr(name, "MEM0.mem32")) {
		dep->board = mem0_board;
	} else if (strstr(name, "SEQ.seq") != NULL) {
		dep->board = seq_board;
	} else if (strstr(name, "TYP.typ") != NULL) {
		dep->board = typ_board;
	} else if (strstr(name, "VAL.val") != NULL) {
		dep->board = val_board;
	} else {
		fprintf(stderr, "Unrecognized DIPROC: %s\n", name);
		exit(2);
	}
}

static void
upload(uint8_t *ptr, const char *hexstr)
{
	uint8_t byte;

	while (*hexstr) {
		while (isspace(*hexstr))
			hexstr++;
		byte = digittoint(*hexstr++) << 4;
		byte |= digittoint(*hexstr++);
		*ptr++ = byte;
	}
}

static int
ioc_board(struct diagproc_exp_priv *dep, uint8_t length)
{
	(void)dep;
	(void)length;
	return (0);
}

static int
fiu_board(struct diagproc_exp_priv *dep, uint8_t length)
{
	struct ctx *ctx;
	uint8_t *dst;
	uint16_t ctr;

	if (length == 0x52 && dep->ram[0x10] == 0x2f) {
		sc_tracef(dep->name, "Exp read_novram_data.fiu");
		upload(dep->ram + 0x22, "2c272b0204b6a1");
		SET_PT(dep);
		return(1);
	}

	if (length == 0x4f && dep->ram[0x10] == 0x38) {
		sc_tracef(dep->name, "Exp load_hram_1.fiu");
		/*
		 * 0x008 0x040 1
		 * 0x004 0x080 2
		 * 0x002 0x100 4
		 * 0x001 0x200 8
		 */
		ctx = CTX_Find("FIU.fiu_55.HRAM1");
		AN(ctx);
		dst = (uint8_t*)(ctx + 1);
		for (ctr = 0; ctr < 1024; ctr++) {
			*dst = 0xf;
			if (((ctr >> 3) ^ (ctr >> 6)) & 1)
				*dst &= ~1;
			if (((ctr >> 2) ^ (ctr >> 7)) & 1)
				*dst &= ~2;
			if (((ctr >> 1) ^ (ctr >> 8)) & 1)
				*dst &= ~4;
			if (((ctr >> 0) ^ (ctr >> 9)) & 1)
				*dst &= ~8;
			dst++;
		}
		return (1);
	}

	if (length == 0x67 && dep->ram[0x10] == 0x3f) {
		sc_tracef(dep->name, "Exp load_hram_32_0.fiu");
		/*
		 * 0x008 0x040 1
		 * 0x004 0x080 2
		 * 0x010 0x100 4
		 * 0x020 0x200 8
		 */
		ctx = CTX_Find("FIU.fiu_55.HRAM0");
		AN(ctx);
		dst = (uint8_t*)(ctx + 1);
		for (ctr = 0; ctr < 1024; ctr++) {
			*dst = 0x0f;
			if (((ctr >> 3) ^ (ctr >> 6)) & 1)
				*dst &= ~1;
			if (((ctr >> 2) ^ (ctr >> 7)) & 1)
				*dst &= ~2;
			if (((ctr >> 4) ^ (ctr >> 8)) & 1)
				*dst &= ~4;
			if (((ctr >> 5) ^ (ctr >> 9)) & 1)
				*dst &= ~8;
			dst++;
		}
		return (1);
	}
	return (0);
}

static int
mem32_board(struct diagproc_exp_priv *dep, uint8_t length, int board)
{
	struct ctx *ctx;
	uint8_t *dst;
	unsigned u;

	(void)board;

	if (length == 0x4b && dep->ram[0x10] == 0x2a) {
		sc_tracef(dep->name, "Exp read_novram_data.m32");
		upload(dep->ram + 0x19, "2f28ee02060000000000b6ee");
		SET_PT(dep);
		return(1);
	}
	if (length == 0x41 && dep->ram[0x10] == 0x22) {
		sc_tracef(dep->name, "Exp clear_tagstore.m32");

		ctx = CTX_Find("MEM0.mem32_09.TAR");
		AN(ctx);
		dst = (uint8_t*)(ctx + 1);
		memset(dst, 0, 0x20000);
		for(u = 0; u < 0x4000; u++)
			dst[u * 8] = 0x7f;

		ctx = CTX_Find("MEM0.mem32_12.TBR");
		AN(ctx);
		dst = (uint8_t*)(ctx + 1);
		memset(dst, 0, 0x20000);
		for(u = 0; u < 0x4000; u++)
			dst[u * 8] = 0x7f;

		return(1);
	}
	return (0);
}

static int
mem0_board(struct diagproc_exp_priv *dep, uint8_t length)
{
	return (mem32_board(dep, length, 0));
}

static int
seq_board(struct diagproc_exp_priv *dep, uint8_t length)
{

	if (length == 0x45 && dep->ram[0x10] == 0x27) {
		sc_tracef(dep->name, "Exp read_novram_data.seq");
		upload(dep->ram + 0x1a, "2d294b0205b82c");
		SET_PT(dep);
		return (1);
	}
	return (0);
}

static uint64_t
get_wdr(struct diagproc_exp_priv *dep, uint8_t offset)
{
	uint64_t wdr, m;
	unsigned v;
	int i;

	//printf("%s %02x: ", dep->name, offset);
	wdr = 0;
	m = 1ULL << 63;
	//for (i = 0; i < 12; i++)
	//	printf("%02x ", dep->ram[offset + i]);
	for (v = 0x80; v > 0x02; v >>= 1) {
		for (i = 11; i >= 0; i--) {
			if (v == 0x20 && i < 4)
				continue; // parity
			if (v == 0x10 && i >= 8)
				continue; // parity
			if (dep->ram[offset + i] & v) {
				//putchar('#');
				wdr |= m;
			} else {
				//putchar('-');
			}
			m >>= 1;
		}
	}
	//printf("=> %016jx\n", (uintmax_t)wdr);
	return (wdr);
}

static int
typ_val_rfload(struct diagproc_exp_priv *dep, uint8_t length, const char *aram, const char *bram, const char *suf)
{
	uint64_t wdr;
	unsigned w;
	struct ctx *ctx;
	uint8_t *dst;

	if (dep->ram[0x10] != 0xd9)
		return(0);

	if (length == 0xdc) {
		// LOAD_REGISTER_FILE_200.TYP (complete download)
		ctx = CTX_Find(aram);
		AN(ctx);
		dst = (uint8_t*)(ctx + 1);
		dep->dst1 = dst;

		ctx = CTX_Find(bram);
		AN(ctx);
		dst = (uint8_t*)(ctx + 1);
		dep->dst2 = dst;
	}
	if (length == 0xdc || length == 0xc8) {
		// LOAD_REGISTER_FILE_200.TYP (complete & modify downloads)
		sc_tracef(dep->name, "Exp load_register_file_200.%s", suf);
		for (w = 0x18; w < 0xd8; w += 12) {
			wdr = get_wdr(dep, w);
			memcpy(dep->dst1, &wdr, sizeof wdr);
			dep->dst1 += 8;
			memcpy(dep->dst2, &wdr, sizeof wdr);
			dep->dst2 += 8;
			fflush(stdout);
		}
		return (1);
	}
	return (0);
}

static int
typ_board(struct diagproc_exp_priv *dep, uint8_t length)
{

	if (length == 0x40 && dep->ram[0x10] == 0x26) {
		sc_tracef(dep->name, "Exp read_novram_data.typ");
		upload(dep->ram + 0x19, "2b29dc0204b796");
		SET_PT(dep);
		return (1);
	}

	if (typ_val_rfload(dep, length, "TYP.typ_16.ARAM", "TYP.typ_18.BRAM", "typ"))
		return (1);

	return (0);
}

static int
val_board(struct diagproc_exp_priv *dep, uint8_t length)
{

	if (length == 0x40 && dep->ram[0x10] == 0x26) {
		sc_tracef(dep->name, "Exp read_novram_data.val");
		upload(dep->ram + 0x19, "2a28060103b796");
		SET_PT(dep);
		return (1);
	}

	if (typ_val_rfload(dep, length, "VAL.val_18.ARAM", "VAL.val_20.BRAM", "val"))
		return (1);

	return (0);
}

int
diagproc_exp_download(struct diagproc_exp_priv *dep, uint8_t length, uint8_t *ram, uint8_t *ip)
{
	uint8_t first = ram[0x10];

	dep->ram = ram;
	dep->ip = ip;
	if (dep->board(dep, length)) {
		ram[0x04] = 1;
	} else {
		sc_tracef(dep->name, "Exp [0x%02x] @0x%02x 0x%02x 0x%02x 0x%02x", length, first, ram[first], ram[first+1], ram[first+2]);
	}
	return (0);
}
