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
#include <string.h>
#include <unistd.h>

#include "Infra/r1000.h"
#include "Infra/vsb.h"
#include "Diag/diag.h"

#include "Infra/elastic.h"

struct i8052 {
	const char			*name;
	uint8_t				unit;
	uint8_t				ram[256];
	struct elastic_subscriber	*esp;
	pthread_t			thread;
};

static void
i8052_tx_diagbus(const struct i8052 *i52, uint8_t x)
{
	Trace(trace_diagbus_bytes, "%s TX %02x", i52->name, x);
	elastic_inject(diag_elastic, &x, 1);
}

static unsigned
i8052_rx_diagbus(struct i8052 *i52)
{
	void *ptr;
	size_t len;
	uint8_t *u8p;
	unsigned retval;

	AN(elastic_subscriber_fetch(&i52->esp, &ptr, &len));
	assert(len == 2);
	u8p = ptr;
	assert(u8p[0] < 2);
	retval = u8p[0]<<8;
	retval |= u8p[1];
	free(ptr);
	return (retval);
}

static void *
i8052_thread(void *priv)
{
	struct i8052 *i52 = priv;
	uint8_t csum, counter, pointer, reply;
	unsigned u;
	uint8_t u8;
	struct vsb *vsb;
	int me;

	vsb = VSB_new_auto();
	AN(vsb);

	reply = 0;
	while (1) {
		u = i8052_rx_diagbus(i52);
		if (!(u & 0x100))
			continue;
		me = (u & 0x1f) == i52->unit;
		if (!me)
			continue;
		u8 = u & 0xff;
		switch (u8 >> 4) {
		case 0x0:
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
			Trace(trace_diagbus_upload,
			    "%s UL%s", i52->name, VSB_data(vsb));
			break;
		case 0x8:
			reply = 5;
			break;
		case 0xa:
			pointer = 0x10;
			csum = 0;
			counter = i8052_rx_diagbus(i52);
			csum += counter;
			VSB_clear(vsb);
			while (counter--) {
				u8 = i8052_rx_diagbus(i52);
				csum += u8;
				i52->ram[pointer++] = u8;
				VSB_printf(vsb, " %02x", u8);
			}
			AZ(VSB_finish(vsb));
			Trace(trace_diagbus_download,
			    "%s DL%s", i52->name, VSB_data(vsb));
			assert (csum == i8052_rx_diagbus(i52));
			break;
		default:
			break;
		}
	}
}

static void
i8052_start(unsigned unit, const char *name)
{
	struct i8052 *i52;

	i52 = calloc(sizeof *i52, 1);
	AN(i52);
	i52->name = name;
	i52->unit = unit;
	i52->esp = elastic_subscribe(diag_elastic, NULL, NULL);
	AZ(pthread_create(&i52->thread, NULL, i8052_thread, i52));
}

void v_matchproto_(cli_func_f)
cli_dummy_diproc(struct cli *cli)
{
	int i;

	for (i = 1; i < cli->ac; i++) {
		if (!strcmp(cli->av[i], "seq"))
			i8052_start(0x2, "i8052.SEQ.2");
		else if (!strcmp(cli->av[i], "fiu"))
			i8052_start(0x3, "i8052.FIU.3");
		else if (!strcmp(cli->av[i], "ioc"))
			i8052_start(0x4, "i8052.IOC.4");
		else if (!strcmp(cli->av[i], "typ"))
			i8052_start(0x6, "i8052.TYP.6");
		else if (!strcmp(cli->av[i], "val"))
			i8052_start(0x7, "i8052.VAL.7");
		else if (!strcmp(cli->av[i], "mem0"))
			i8052_start(0xc, "i8052.MEM0.e");
		else if (!strcmp(cli->av[i], "mem1"))
			i8052_start(0xc, "i8052.MEM1.d");
		else if (!strcmp(cli->av[i], "mem2"))
			i8052_start(0xe, "i8052.MEM2.c");
		else if (!strcmp(cli->av[i], "mem3"))
			i8052_start(0xc, "i8052.MEM3.f");
		else
			cli_printf(cli,
			    "Usage: dummy_diproc "
			    "[seq|fiu|ioc|typ|val|mem0|mem2]…\n");
	}
}
