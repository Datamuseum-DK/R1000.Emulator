/*-
 * Copyright (c) 2021 Poul-Henning Kamp
 * All rights reserved.
 *
 * Author: Poul-Henning Kamp <phk@phk.freebsd.dk>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <stdlib.h>
#include <unistd.h>

#include "r1000.h"
#include "vqueue.h"

#include "elastic.h"

static const char * const STATE_CMD = "{CMD}";
static const char * const STATE_OTHER_DOWNLOAD = "{OTHER_DOWNLOAD}";
static const char * const STATE_MY_DOWNLOAD = "{MY_DOWNLOAD}";
static const char * const STATE_IGNORE = "{IGNORE}";
static const char * const STATE_DOWNLOAD = "{DOWNLOAD}";
static const char * const STATE_UPLOAD = "{UPLOAD}";

struct i8052 {
	const char			*name;
	uint8_t				unit;
	uint8_t				ram[256];
	struct elastic_subscriber	*esp;
	const char			*state;
	const char			*state_after_download;
	uint8_t				counter;
	uint8_t				pointer;
	uint8_t				resp_0;
	int				adhoc;
	uint8_t				txsum;
};

static void
i8052_tx(struct i8052 *i52, uint8_t x)
{
	trace(TRACE_DIAG, "%s %s tx %02x\n", i52->name, i52->state, x);
	i52->txsum += x;
	elastic_inject(diag_elastic, &x, 1);
}

static void
i8052_diag_rx(void *priv, const void * ptr, size_t len)
{
	struct i8052 *i52 = priv;
	uint8_t u8 = *(const uint8_t *)ptr;

	AN(priv);
	AN(ptr);
	assert(len == 1);
	if (i52->state == STATE_CMD) {
		switch (u8 >> 4) {
		case 0x0:
			if ((u8 & 0xf) != i52->unit)
				return;
			trace(TRACE_DIAG, "%s %s got %02x\n", i52->name, i52->state, u8);
			if (i52->adhoc) {
				i8052_tx(i52, i52->adhoc);
				i52->adhoc = 0;
			} else {
				i8052_tx(i52, i52->resp_0);
			}
			break;
		case 0x2:
			if ((u8 & 0xf) != i52->unit) {
				i52->counter = 2;
				i52->state = STATE_IGNORE;
			} else {
				i52->counter = 2;
				i52->state = STATE_DOWNLOAD;
				i52->pointer = 0x8;
				i52->state_after_download = STATE_UPLOAD;
			}
			break;
		case 0x8:
			if ((u8 & 0xf) == i52->unit && i52->resp_0 != 9) {
				trace(TRACE_DIAG, "%s %s Guessing Reset %02x\n", i52->name, i52->state, u8);
				i52->adhoc = 5;
			}
			break;
		case 0xa:
			if ((u8 & 0xf) != i52->unit) {
				i52->state = STATE_OTHER_DOWNLOAD;
			} else {
				trace(TRACE_DIAG, "%s %s Download %02x\n", i52->name, i52->state, u8);
				i52->state = STATE_MY_DOWNLOAD;
				i52->state_after_download = STATE_CMD;
			}
			break;
		default:
			if ((u8 & 0xf) != i52->unit)
				return;
			trace(TRACE_DIAG, "%s %s Ignoring %02x\n", i52->name, i52->state, u8);
			break;
		}
	} else if (i52->state == STATE_OTHER_DOWNLOAD) {
		AN(u8);
		i52->counter = u8 + 1;
		i52->state = STATE_IGNORE;
	} else if (i52->state == STATE_IGNORE) {
		if (!--i52->counter)
			i52->state = STATE_CMD;
	} else if (i52->state == STATE_MY_DOWNLOAD) {
		AN(u8);
		trace(TRACE_DIAG, "%s %s Will download %02x\n", i52->name, i52->state, u8);
		i52->counter = u8 + 1;
		i52->pointer = 0x10;
		i52->state = STATE_DOWNLOAD;
	} else if (i52->state == STATE_DOWNLOAD) {
		trace(TRACE_DIAG, "%s %s Downloading %02x @ %02x\n", i52->name, i52->state, u8, i52->pointer);
		i52->ram[i52->pointer++] = u8;
		if (!--i52->counter) {
			i52->adhoc = 0;
			trace(TRACE_DIAG, "%s %s Download complete\n", i52->name, i52->state);
			i52->state = i52->state_after_download;
			i52->resp_0 = 1;
		}
	}
	if (i52->state == STATE_UPLOAD) {
		trace(TRACE_DIAG, "%s %s [%02x %02x] Uploading\n", i52->name, i52->state, i52->ram[8], i52->ram[9]);
		i52->txsum = 0;
		i52->ram[0x11] = 0x02;
		i8052_tx(i52, i52->ram[0x11]);
		i52->pointer = i52->ram[8];
		for (u8 = 0; u8 < i52->ram[9]; u8++)
			i8052_tx(i52, i52->ram[i52->pointer++]);
		i8052_tx(i52, i52->txsum);
		i52->state = STATE_CMD;
	}
}

static void
i8052_start(unsigned unit, uint8_t resp_0, const char *name)
{
	struct i8052 *i52;

	i52 = calloc(sizeof *i52, 1);
	AN(i52);
	i52->name = name;
	i52->unit = unit;
	i52->resp_0 = resp_0;
	i52->state = STATE_CMD;
	i52->esp = elastic_subscribe(diag_elastic, i8052_diag_rx, i52);
}

void
i8052_init(void)
{
	AN(diag_elastic);
	i8052_start(0x2, 5, "i8052.SEQ.2");
	i8052_start(0x3, 5, "i8052.FIU.3");
	i8052_start(0x4, 5, "i8052.IOC.4");
	i8052_start(0x6, 5, "i8052.TYP.6");
	i8052_start(0x7, 5, "i8052.VAL.7");
	i8052_start(0xc, 5, "i8052.MEM0.c");
	i8052_start(0xd, 9, "i8052.MEM1.d");
	i8052_start(0xe, 9, "i8052.MEM2.e");
	i8052_start(0xf, 9, "i8052.MEM3.f");
}
