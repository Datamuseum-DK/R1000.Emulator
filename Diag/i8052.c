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
#include "vsb.h"

#include "elastic.h"

struct i8052 {
	const char			*name;
	uint8_t				unit;
	uint8_t				ram[256];
	struct elastic_subscriber	*esp;
	uint8_t				present;
	pthread_t			thread;
};

static void
i8052_tx_diagbus(const struct i8052 *i52, uint8_t x)
{
	(void)i52;
	elastic_inject(diag_elastic, &x, 1);
}

static uint8_t
i8052_rx_diagbus(struct i8052 *i52)
{
	void *ptr;
	size_t len;
	uint8_t u8;

	AN(elastic_subscriber_fetch(&i52->esp, &ptr, &len));
	assert(len == 1);
	u8 = *(uint8_t*)ptr;
	free(ptr);
	return (u8);
}

static void *
i8052_thread(void *priv)
{
	struct i8052 *i52 = priv;
	uint8_t csum, counter, pointer, reply;
	uint8_t u8;
	struct vsb *vsb;
	int me;

	vsb = VSB_new_auto();
	AN(vsb);

	reply = 0;
	while (1) {
		u8 = i8052_rx_diagbus(i52);
		me = (u8 & 0xf) == i52->unit;
		switch (u8 >> 4) {
		case 0x0:
			if (!me)
				break;
			if (reply) {
				i8052_tx_diagbus(i52, reply);
				reply = 0;
			} else {
				i8052_tx_diagbus(i52, 1);
			}
			break;
		case 0x2:
			pointer = i8052_rx_diagbus(i52);
			counter = i8052_rx_diagbus(i52);
			if (!me)
				break;
			csum = 0;
			i52->ram[0x11] = 0x02;
			VSB_clear(vsb);
			VSB_printf(vsb, " %02x", i52->ram[0x11]);
			i8052_tx_diagbus(i52, i52->ram[0x11]);
			csum += i52->ram[0x11];
			while (counter--) {
				u8 = i52->ram[pointer++];
				csum += u8;
				i8052_tx_diagbus(i52, u8);
				VSB_printf(vsb, " %02x", u8);
			}
			i8052_tx_diagbus(i52, csum);
			AZ(VSB_finish(vsb));
			trace(TRACE_DIAG, "%s UL%s\n", i52->name, VSB_data(vsb));
			break;
		case 0x8:
			if (!me || !i52->present)
				break;
			reply = 5;
			break;
		case 0xa:
			pointer = 0x10;
			csum = 0;
			counter = i8052_rx_diagbus(i52);
			csum += counter;
			if (!me) {
				while (counter--)
					csum += i8052_rx_diagbus(i52);
			} else {
				VSB_clear(vsb);
				while (counter--) {
					u8 = i8052_rx_diagbus(i52);
					csum += u8;
					i52->ram[pointer++] = u8;
					VSB_printf(vsb, " %02x", u8);
				}
				AZ(VSB_finish(vsb));
				trace(TRACE_DIAG, "%s DL%s\n", i52->name, VSB_data(vsb));
			}
			assert (csum == i8052_rx_diagbus(i52));
			break;
		default:
			if (!me)
				break;
			trace(TRACE_DIAG, "%s Ignoring %02x\n", i52->name, u8);
			break;
		}
	}
	return (NULL);
}

static void
i8052_start(unsigned unit, uint8_t present, const char *name)
{
	struct i8052 *i52;

	i52 = calloc(sizeof *i52, 1);
	AN(i52);
	i52->name = name;
	i52->unit = unit;
	i52->present = present;
	i52->esp = elastic_subscribe(diag_elastic, NULL, NULL);
	AZ(pthread_create(&i52->thread, NULL, i8052_thread, i52));
}

void
i8052_init(void)
{
	AN(diag_elastic);
	i8052_start(0x2, 1, "i8052.SEQ.2");
	i8052_start(0x3, 1, "i8052.FIU.3");
	i8052_start(0x4, 1, "i8052.IOC.4");
	i8052_start(0x6, 1, "i8052.TYP.6");
	i8052_start(0x7, 1, "i8052.VAL.7");
	i8052_start(0xc, 1, "i8052.MEM0.c");

	/*
	 * XXX: Who answers for MEM boards not in their slots ?
	 * XXX: Sending no reply does not seem to lead to timeout.
	 */

	i8052_start(0xd, 0, "i8052.MEM1.d");
	i8052_start(0xe, 0, "i8052.MEM2.e");
	i8052_start(0xf, 0, "i8052.MEM3.f");
}
