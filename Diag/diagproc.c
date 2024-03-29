
#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Infra/r1000.h"
#include "Chassis/r1000sc.h"
#include "Diag/diagproc.h"
#include "Diag/i8052_emul.h"
#include "Infra/elastic.h"
#include "Infra/vsb.h"
#include "Diag/diag.h"

#define FLAG_RX_SPIN		(1)
#define FLAG_DOWNLOAD		(1<<1)
#define FLAG_RX_DISABLE		(1<<2)
#define FLAG_IDLE		(1<<3)
#define FLAG_WAIT_DFSM		(1<<4)
#define FLAG_NOT_CODE		(1<<5)
#define FLAG_DUMP_MEM		(1<<6)
#define FLAG_LONGWAIT_DFSM	(1<<7)
#define FLAG_DOWNLOAD_LEN	(1<<8)
#define FLAG_DOWNLOAD_RUN	(1<<9)
#define FLAG_UPLOAD		(1<<10)

struct diagproc_priv {
	char *name;
	char *arg;
	unsigned mod;
	struct mcs51 *mcs51;
	int version;
	int idle;
	uint32_t *do_trace;
	struct elastic_subscriber *diag_bus;
	pthread_mutex_t mtx;
	int did_io;
	int longwait;
	struct vsb *vsb;

	int8_t pc0;
	unsigned flags[0x2000];
	uint8_t download_len;

	struct diagproc_exp_priv *exp;

	uint8_t dl_ptr;
	uint8_t dl_cnt;
	uint8_t dl_sum;
};

static void
diagproc_movx8_write(struct mcs51 *mcs51, uint8_t adr, int data)
{
	struct diagproc_ctrl *dc;
	struct diagproc_priv *dp;

	assert(mcs51->priv != NULL);
	dc = mcs51->priv;
	assert(dc->priv != NULL);
	dp = dc->priv;
	assert(dp->mcs51 != NULL);
	assert(dp->mcs51 == mcs51);
	dp->did_io = 1;

	dc->do_movx = 1;
	dc->movx_data = data;
	dc->movx_adr = adr;

}

static unsigned
diagproc_sfrfunc(struct mcs51 * mcs51, uint8_t sfr_adr, int what)
{
	struct diagproc_ctrl *dc;
	struct diagproc_priv *dp;
	unsigned retval = 999;
	uint8_t txbuf[1];

	assert(mcs51->priv != NULL);
	dc = mcs51->priv;
	assert(dc->priv != NULL);
	dp = dc->priv;
	assert(dp->mcs51 != NULL);
	assert(dp->mcs51 == mcs51);
	dp->did_io = 1;

	switch (sfr_adr) {
	case SFR_P1:
		if (what < 0) {
			retval = dc->p1val;
		} else {
			dc->p1mask = 0xff;
			dc->p1val = what;
			retval = what;
		}
		break;
	case SFR_SBUF:
		if (what >= 0) {
			Trace(trace_diagbus, "%s TX %02x", dp->name, what);
			txbuf[0] = what;
			elastic_inject(diag_elastic, txbuf, 1);
			dp->mcs51->sfr[SFR_SCON] |= 0x2;
			retval = what;
		} else {
			retval = dp->mcs51->sfr[SFR_SBUF];
			dp->mcs51->sfr[SFR_SCON] &= ~0x01;
			if (*dp->do_trace & 8)
				sc_tracef(dp->name, "RX %02x", retval);
		}
		break;
	case SFR_P2:
		if (what < 0) {
			retval = dc->p2val;
		} else {
			dc->p2mask = 0xff;
			dc->p2val = what;
			retval = what;
		}
		break;
	default:
		break;
	}
	if (*dp->do_trace & 2) {
		if (retval == 999)
			sc_tracef(dp->name, "WRONG SfrFunc(0x%x, %d)",
			    sfr_adr, what);
		else
			sc_tracef(dp->name, "SfrFunc(0x%x, %d) => 0x%02x",
			    sfr_adr, what, retval);
	}
	return (retval);
}

static unsigned
diagproc_bitfunc(struct mcs51 *mcs51, uint8_t bit_adr, int what)
{
	struct diagproc_ctrl *dc;
	struct diagproc_priv *dp;
	unsigned retval = 9;

	assert(mcs51->priv != NULL);
	dc = mcs51->priv;
	assert(dc->priv != NULL);
	dp = dc->priv;
	assert(dp->mcs51 != NULL);
	assert(dp->mcs51 == mcs51);
	dp->did_io = 1;

	switch (bit_adr & ~7) {
	case SFR_TCON:
		dp->did_io = 1;

		// XXX: Disable DELAY
		if ((bit_adr & 7) == 5 && what == -1)
			return (1);

		return (mcs51_bitfunc_default(mcs51, bit_adr, what));
	case SFR_P1:
		if (what < 0) {
			retval = (dc->p1val >> (bit_adr & 7)) & 1;
		} else {
			dc->p1mask |= 1 << (bit_adr & 7);
			if (what == 0)
				dc->p1val &= ~(1U << (bit_adr & 7));
			else
				dc->p1val |= (1U << (bit_adr & 7));
			retval = 0;
		}
		break;
	case SFR_P3:
		if (what < 0) {
			retval = (dc->p3val >> (bit_adr & 7)) & 1;
		} else {
			dc->p3mask |= 1 << (bit_adr & 7);
			if (what == 0)
				dc->p3val &= ~(1U << (bit_adr & 7));
			else
				dc->p3val |= (1U << (bit_adr & 7));
			retval = 0;
		}
		break;
	default:
		WRONG();
	}
	if (*dp->do_trace & 2) {
		if (retval == 9)
			sc_tracef(dp->name, "WRONG BitFunc(0x%x, %d)",
			    bit_adr, what);
		else
			sc_tracef(dp->name, "BitFunc(0x%x, %d) => %u",
			    bit_adr, what, retval);
	}
	return (retval);
}

static void
diagproc_fast_dload(struct diagproc_priv *dp, const uint8_t *ptr)
{
	if (dp->dl_cnt == 0 && dp->dl_ptr == 0 && dp->dl_sum == 0) {
		assert(ptr[0] == 1);
		assert((ptr[1] & 0xe0) == 0xa0);
		dp->dl_cnt = 1;
	} else if (dp->dl_cnt == 1 && dp->dl_ptr == 0 && dp->dl_sum == 0) {
		dp->dl_cnt = ptr[1] + 1;
		dp->download_len = ptr[1];
		dp->dl_ptr = 0x10;
		dp->dl_sum += ptr[1];
	} else if (dp->dl_cnt > 1 && dp->dl_ptr > 0) {
		dp->dl_sum += ptr[1];
		dp->mcs51->iram[dp->dl_ptr] = ptr[1];
		dp->dl_cnt--;
		dp->dl_ptr++;
	} else if (dp->dl_ptr > 0) {
		assert(dp->dl_sum == ptr[1]);
		dp->dl_cnt = 0;
		dp->dl_ptr = 0;
		dp->dl_sum = 0;

		if (dp->mcs51->iram[0x11] & 0x01)
			dp->mcs51->sfr[SFR_IP] |= 0x01;
		else
			dp->mcs51->sfr[SFR_IP] &= ~0x01;

		if (dp->mcs51->iram[0x11] & 0x02)
			dp->mcs51->sfr[SFR_IP] |= 0x02;
		else
			dp->mcs51->sfr[SFR_IP] &= ~0x02;

		if ((dp->mod & 0x10) && diagproc_exp_download(dp->exp, dp->download_len, dp->mcs51->iram, &dp->mcs51->sfr[SFR_IP])) {
			// pass
		} else {
			dp->pc0 = dp->mcs51->iram[0x10];
			dp->mcs51->iram[0x04] = 0x06;
		}
	}
}


static void
diagproc_busrx(void *priv, const void *ptr, size_t len)
{
	struct diagproc_priv *dp = priv;
	uint8_t serbuf[2];
	int i, fast = 0;

	assert(len == 2);
	memcpy(serbuf, ptr, len);

	assert(pthread_mutex_lock(&dp->mtx) == 0);

	if (dp->dl_cnt) {
		fast = 1;
	} else if (!serbuf[0]) {
		// pass
	} else if ((serbuf[1] & 0x1f) != dp->mcs51->iram[3]) {
		// pass
	} else if ((serbuf[1] & 0xe0) != 0xa0) {
		// pass
	} else if (dp->mcs51->iram[4] == DIPROC_RESPONSE_RUNNING) {
		// pass
	} else {
		fast = 1;
	}
	if (fast) {
		while (!dp->idle) {
			assert(pthread_mutex_unlock(&dp->mtx) == 0);
			usleep(1000);
			assert(pthread_mutex_lock(&dp->mtx) == 0);
		}
		diagproc_fast_dload(dp, serbuf);
	} else {
		/*
		 * The DIPROC firmware reads SBUF twice, at both 0x649 and 0x64d, so we cannot
		 * use the SCON.RI bit for flow-control.
		 * Instead, wait if:
		 *	Interrupted and not spinning in 0x646 or 0x6e3
		 *	or if SCON.RI is already set
		 *	of if SCON.REN is not set
		 */
		while (
		    (dp->mcs51->irq_state && !(dp->flags[dp->mcs51->pc] & FLAG_RX_SPIN)) ||
		    (dp->mcs51->sfr[SFR_SCON] & 0x01) ||
		    (!(dp->mcs51->sfr[SFR_SCON] & 0x10))) {
			assert(pthread_mutex_unlock(&dp->mtx) == 0);
			usleep(1000);
			assert(pthread_mutex_lock(&dp->mtx) == 0);
		}
		MCS51_Rx(dp->mcs51, serbuf[1], serbuf[0]);
	}
	i = dp->mcs51->iram[3];
	if (0 < i && i < 16)
		diprocs[i].status = dp->mcs51->iram[4];
	assert(pthread_mutex_unlock(&dp->mtx) == 0);
}

static uint16_t
diagproc_istep(struct diagproc_ctrl *dc, struct diagproc_context *dctx)
{
	struct diagproc_priv *dp;
	uint16_t opc, npc;
	unsigned ptr, u, v;
	uint16_t flags;

	assert(dc != NULL);
	assert(dc->priv != NULL);
	dp = dc->priv;

	dp->did_io = 0;

	dc->next_needs_p1 = 0;
	dc->next_needs_p2 = 0;
	dc->next_needs_p3 = 0;

	dp->mcs51->do_trace = *dp->do_trace;
	opc = dp->mcs51->pc;
	npc = MCS51_SingleStep(dp->mcs51);
	dctx->instructions++;

	assert(npc < 0x2000);
	flags = dp->flags[npc];
	if (flags & FLAG_DOWNLOAD_LEN)
		dp->download_len = dp->mcs51->sfr[SFR_ACC];
	if (flags & FLAG_DOWNLOAD)
		dp->pc0 = dp->mcs51->iram[0x10];
	if (flags & FLAG_DOWNLOAD_RUN) {
		if (*dp->do_trace & 16) {
			VSB_clear(dp->vsb);
			VSB_printf(dp->vsb, "%02x ", dp->download_len);
			for (u = 0; u < dp->download_len - 1; u++)
				VSB_printf(dp->vsb, "%02x", dp->mcs51->iram[0x10 + u]);
			AZ(VSB_finish(dp->vsb));
			sc_tracef(dp->name, "Download %s", VSB_data(dp->vsb));
		}
		if (dp->mod & 0x10) {
			(void)diagproc_exp_download(dp->exp, dp->download_len, dp->mcs51->iram, &dp->mcs51->sfr[SFR_IP]);
		}
	}
	if (flags & FLAG_UPLOAD) {
		if (*dp->do_trace & 16) {
			VSB_clear(dp->vsb);
			v = dp->mcs51->iram[0x00];
			VSB_printf(dp->vsb, "%02x ", v);
			VSB_printf(dp->vsb, "%02x ", dp->mcs51->iram[0x01]);
			for (u = 0; u < dp->mcs51->iram[0x01]; u++)
				VSB_printf(dp->vsb, "%02x", dp->mcs51->iram[v + u]);
			AZ(VSB_finish(dp->vsb));
			sc_tracef(dp->name, "Upload %s", VSB_data(dp->vsb));
		}
	}

	if (flags & FLAG_RX_DISABLE) {
		/*
		 * The DIAGBUS 'reset' command writes to SCON potentially
		 * erasing a 0x01 bit.  Disable reception before RETI
		 * to close this race.
		 */
		dp->mcs51->sfr[SFR_SCON] &= ~0x10;
	}

	if (dp->flags[npc] & (FLAG_IDLE|FLAG_RX_SPIN))
		dp->idle++;
	else
		dp->idle = 0;
	if ((*dp->do_trace & 2) && dp->idle < 5)
		sc_tracef(dp->name, "Instr 0x%04x %02x:%s\t|nPC 0x%04x/%d",
		    dp->mcs51->pc, dp->mcs51->progmem[dp->mcs51->pc],
		    dp->mcs51->tracebuf, npc, dp->idle
		);
	if (dp->flags[npc] & FLAG_DUMP_MEM) {
		dctx->executions++;
		if (*dp->do_trace & 4) {
			VSB_clear(dp->vsb);
			for (ptr = 0x10; ptr < dp->pc0 + 16U && ptr < 0x100U; ptr++) {
				if (!(ptr & 3))
					VSB_putc(dp->vsb, ' ');
				VSB_printf(dp->vsb, " %02x", dp->mcs51->iram[ptr]);
			}
			AZ(VSB_finish(dp->vsb));

			ptr = MCS51_REG(dp->mcs51, 0);
			sc_tracef(dp->name, "Exec %02x | %02x %02x %02x %02x | %s",
			    ptr,
			    dp->mcs51->iram[ptr],
			    dp->mcs51->iram[ptr + 1],
			    dp->mcs51->iram[ptr + 2],
			    dp->mcs51->iram[ptr + 3],
			    VSB_data(dp->vsb)
			);
		}
	}
	dp->mcs51->pc = npc;
	if (dp->flags[npc] & FLAG_NOT_CODE) {
		sc_tracef(dp->name, "OUT OF PROGRAM next PC 0x%04x", npc);
		exit(2);
	}
	dc->next_needs_p1 = 1;
	dc->next_needs_p2 = 1;
	dc->next_needs_p3 = 1;
	return (opc);
}

void
DiagProcStep(struct diagproc_ctrl *dc, struct diagproc_context *dctx)
{
	struct diagproc_priv *dp;
	uint16_t retval;
	uint16_t flags;
	int i;

	assert(dc != NULL);
	assert(dctx != NULL);
	assert(dc->priv != NULL);
	dp = dc->priv;

	if (dc->pin9_reset) {
		Trace(trace_diagbus, "%s RST", dp->name);
		MCS51_Reset(dp->mcs51);
		return;
	}

	MCS51_TimerTick(dp->mcs51);

	if (dp->longwait > 0) {
		dp->longwait--;
		return;
	}

	do {
		assert(dp->mcs51->pc < 0x2000);
		flags = dp->flags[dp->mcs51->pc];
		if ((dc->p3val & 0x08) && (flags & FLAG_WAIT_DFSM))
			return;
		assert(pthread_mutex_lock(&dp->mtx) == 0);
		retval = diagproc_istep(dc, dctx);
		dctx->profile[retval]++;
		assert(pthread_mutex_unlock(&dp->mtx) == 0);
		assert(retval < 0x2000);
		flags = dp->flags[retval];
		if (flags & FLAG_LONGWAIT_DFSM) {
			dp->longwait = 5;
			break;
		}
		i = dp->mcs51->iram[3];
		if (0 < i && i < 16)
			diprocs[i].status = dp->mcs51->iram[4];
	} while(!(flags & (FLAG_IDLE | FLAG_RX_SPIN)) && !dp->did_io);
}

static void
diagproc_set_serialflags(struct diagproc_priv *dp, unsigned serial_rx_byte)
{
	dp->flags[serial_rx_byte] |= FLAG_RX_SPIN;
	dp->flags[serial_rx_byte + 0x94] |= FLAG_DOWNLOAD_LEN;
	dp->flags[serial_rx_byte + 0x9d] |= FLAG_RX_SPIN;
	dp->flags[serial_rx_byte + 0xaf] |= FLAG_DOWNLOAD;
	dp->flags[serial_rx_byte + 0xb2] |= FLAG_DOWNLOAD_RUN;
	dp->flags[serial_rx_byte + 0xc1] |= FLAG_UPLOAD;
	dp->flags[serial_rx_byte + 0xf7] |= FLAG_RX_DISABLE;

}

struct diagproc_ctrl *
DiagProcCreate(const char *name, const char *arg, uint32_t *do_trace)
{
	struct diagproc_priv *dp;
	struct diagproc_ctrl *dc;
	uint8_t firmware[8192];
	unsigned u;
	char *p, *q;

	dp = calloc(sizeof *dp, 1);
	assert(dp != NULL);

	assert(pthread_mutex_init(&dp->mtx, NULL) == 0);

	dc = calloc(sizeof *dc, 1);
	assert(dc != NULL);
	dc->priv = dp;

	dp->vsb = VSB_new_auto();
	AN(dp->vsb);

	dp->name = strdup(name);
	assert(dp->name != NULL);

	dp->arg = strdup(arg);
	assert(dp->arg != NULL);

	p = strstr(dp->arg, "mod");
	if (p != NULL) {
		q = NULL;
		dp->mod = strtoul(p + 3, &q, 0);
		assert(q != NULL);
		assert(*q == '\0');
		printf("%s MOD %u\n", name, dp->mod);
	}

	if (dp->mod & 0x10)
		diagproc_exp_init(&dp->exp, name);

	dp->mcs51 = MCS51_Create(name);
	assert(dp->mcs51 != NULL);
	dp->mcs51->do_trace = *do_trace;
	dp->mcs51->priv = dc;
	dp->mcs51->movx8_write = diagproc_movx8_write;

	dp->do_trace = do_trace;

	MCS51_SetSFR(dp->mcs51, SFR_P1, diagproc_sfrfunc, "P1");
	MCS51_SetSFRBits(dp->mcs51, SFR_P1, diagproc_bitfunc,
	    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	MCS51_SetSFR(dp->mcs51, SFR_P2, diagproc_sfrfunc, "P2");
	MCS51_SetSFRBits(dp->mcs51, SFR_P2, diagproc_bitfunc,
	    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	MCS51_SetSFR(dp->mcs51, SFR_P3, diagproc_sfrfunc, "P3");
	MCS51_SetSFRBits(dp->mcs51, SFR_P3, diagproc_bitfunc,
	    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	MCS51_SetSFRBits(dp->mcs51, SFR_TCON, diagproc_bitfunc,
	    "TF1", "TR1", "TF0", "TR0", "IE1", "IT1", "IE0", "IT0");

	MCS51_SetSFR(dp->mcs51, SFR_SBUF, diagproc_sfrfunc, "SBUF");

	if (strstr(name, "MEM") == NULL) {
		dp->version = 1;
		load_programmable(name, firmware, sizeof firmware, "P8052AH_9028");
		diproc1_mod(firmware, dp->mod);
		diagproc_set_serialflags(dp, 0x646);
		dp->flags[0x17d] |= FLAG_IDLE;
		dp->flags[0x17f] |= FLAG_IDLE;
		dp->flags[0x51d] |= FLAG_DUMP_MEM;
		dp->flags[0x1180] |= FLAG_WAIT_DFSM;
		dp->flags[0x1186] |= FLAG_WAIT_DFSM;
		dp->flags[0x1198] |= FLAG_WAIT_DFSM;    // TEST_INC_MAR.FIU
		dp->flags[0x11a2] |= FLAG_WAIT_DFSM;
		dp->flags[0x122d] |= FLAG_WAIT_DFSM;    // TEST_RDR_SCAN.IOC
		dp->flags[0x1233] |= FLAG_WAIT_DFSM;
		dp->flags[0x123f] |= FLAG_WAIT_DFSM;
		dp->flags[0x1251] |= FLAG_WAIT_DFSM;    // TEST_INC_MAR.FIU
		dp->flags[0x1259] |= FLAG_WAIT_DFSM;    // TEST_INC_MAR.FIU
		dp->flags[0x1262] |= FLAG_WAIT_DFSM;    // TEST_INC_MAR.FIU
		dp->flags[0x1262] |= FLAG_LONGWAIT_DFSM;    // LATCHED_STACK_BIT_1_FRU.SEQ
		dp->flags[0x12c6] |= FLAG_WAIT_DFSM;
		dp->flags[0x12d9] |= FLAG_WAIT_DFSM;
		dp->flags[0x12e5] |= FLAG_WAIT_DFSM;    // TEST_UIR.FIU
		dp->flags[0x1345] |= FLAG_WAIT_DFSM;
		dp->flags[0x1367] |= FLAG_WAIT_DFSM;
		dp->flags[0x1380] |= FLAG_WAIT_DFSM;
		for (u = 0x1426; u < sizeof dp->flags / sizeof dp->flags[0]; u++)
			dp->flags[u] |= FLAG_NOT_CODE;
	} else {
		dp->version = 2;
		load_programmable(name, firmware, sizeof firmware, "DIPROC-01");
		diagproc_set_serialflags(dp, 0x69f);
		dp->flags[0x19d] |= FLAG_IDLE;
		dp->flags[0x19f] |= FLAG_IDLE;
		dp->flags[0x56f] |= FLAG_DUMP_MEM;
		dp->flags[0x10a9] |= FLAG_WAIT_DFSM;
		dp->flags[0x10d8] |= FLAG_WAIT_DFSM;
		dp->flags[0x1109] |= FLAG_WAIT_DFSM;
		for (u = 0x11c9; u < sizeof dp->flags / sizeof dp->flags[0]; u++)
			dp->flags[u] |= FLAG_NOT_CODE;
	}

	AZ(MCS51_SetProgMem(dp->mcs51, firmware, sizeof firmware));

	dp->diag_bus = elastic_subscribe(diag_elastic, diagproc_busrx, dp);

	sc_tracef(dp->name, "DIAGPROC Instantiated (mod 0x%x)", dp->mod);
	return (dc);
}
