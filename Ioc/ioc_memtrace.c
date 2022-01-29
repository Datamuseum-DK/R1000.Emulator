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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Infra/r1000.h"
#include "Ioc/ioc.h"
#include "Ioc/memspace.h"

struct memtrace {
	int			number;
	const char		*seg;
	char			*lo;
	char			*hi;
	VTAILQ_ENTRY(memtrace)	list;
};

static VTAILQ_HEAD(memtracehead, memtrace) memtraces =
    VTAILQ_HEAD_INITIALIZER(memtraces);

static int v_matchproto_(mem_event_f)
memtrace_event(void *priv, const struct memdesc *md, const char *what,
    unsigned adr, unsigned val, unsigned width, unsigned peg)
{
	char buf[16];

	(void)priv;
	(void)peg;

	if (width == 1)
		bprintf(buf, "0x%02x", val);
	else if (width == 2)
		bprintf(buf, "0x%04x", val);
	else
		bprintf(buf, "0x%08x", val);

	if (what == mem_op_read) {
		Trace(1, "MEMTRACE PC %08x %d R 0x%08x %s 0x%x@%s",
		    ioc_pc, ioc_fc, adr, buf, adr - md->lo, md->name);
	} else if (what == mem_op_write) {
		Trace(1, "MEMTRACE PC %08x %d W 0x%08x %s 0x%x@%s",
		    ioc_pc, ioc_fc, adr, buf, adr - md->lo, md->name);
	}
	return (0);
}

#define USAGE \
    "Usage:\n" \
    "\t'add' [-seg name] [-lo adr -hi adr]\n" \
    "\t'del' index\n"

void v_matchproto_(cli_func_f)
cli_ioc_memtrace(struct cli *cli)
{
	struct memtrace *mt1;
	const struct memdesc *md;
	unsigned u, lo, hi;
	int n;

	AN(cli);
	(void)(memtrace_event);
	if (cli->help) {
		cli_printf(cli, USAGE);
		return;
	}
	cli->ac--;
	cli->av++;
	if (cli->ac == 0) {
		if (VTAILQ_EMPTY(&memtraces)) {
			cli_printf(cli, "No active memory traces\n");
			return;
		}
		VTAILQ_FOREACH(mt1, &memtraces, list) {
			cli_printf(cli, "    %d:", mt1->number);
			if (mt1->seg != NULL)
				cli_printf(cli, " -seg %s", mt1->seg);
			if (mt1->lo != NULL)
				cli_printf(cli, " -lo %s", mt1->lo);
			if (mt1->hi != NULL)
				cli_printf(cli, " -lo %s", mt1->hi);
			cli_printf(cli, "\n");
		}
		return;
	} else if (!strcmp(cli->av[0], "add")) {
		cli->ac--;
		cli->av++;
		if (cli_n_m_args(cli, 2, 4, ""))
			return;
		mt1 = VTAILQ_LAST(&memtraces, memtracehead);
		if (mt1 != NULL)
			n = mt1->number + 1;
		else
			n = 1;
		mt1 = calloc(sizeof *mt1, 1);
		AN(mt1);
		mt1->number = n;
		if (!strcmp(cli->av[0], "-seg")) {
			cli->ac--;
			cli->av++;
			if (cli_n_m_args(cli, 1, 1, "")) {
				free(mt1);
				return;
			}
			for (u = 0; u < n_memdesc; u++) {
				if (!strcmp(memdesc[u]->name, cli->av[0]))
					break;
			}
			if (u == n_memdesc) {
				free(mt1);
				cli_error(cli, "Unknow segment name\n");
				return;
			}
			md = memdesc[u];
			mt1->seg = md->name;
			mem_peg_register(md->lo, md->hi, memtrace_event, mt1);
			VTAILQ_INSERT_TAIL(&memtraces, mt1, list);
			return;
		}
		if (!strcmp(cli->av[0], "-lo")) {
			cli->ac--;
			cli->av++;
			if (cli_n_m_args(cli, 3, 3, "")) {
				free(mt1);
				return;
			}
			lo = strtoul(cli->av[0], NULL, 0);
			mt1->lo = strdup(cli->av[0]);
			AN(mt1->lo);
			cli->ac--;
			cli->av++;
			if (strcmp(cli->av[0], "-hi")) {
				free(mt1);
				cli_error(cli, USAGE);
				return;
			}
			cli->ac--;
			cli->av++;
			hi = strtoul(cli->av[0], NULL, 0);
			mt1->hi = strdup(cli->av[0]);
			AN(mt1->hi);
			mem_peg_register(lo, hi, memtrace_event, mt1);
			VTAILQ_INSERT_TAIL(&memtraces, mt1, list);
			return;
		}
		free(mt1);
		(void)cli_error(cli, USAGE);
	} else if (!strcmp(cli->av[0], "del")) {
		cli->ac--;
		cli->av++;
		if (cli_n_m_args(cli, 1, 1, ""))
			return;
		n = strtoul(cli->av[0], NULL, 0);
		VTAILQ_FOREACH(mt1, &memtraces, list) {
			if (mt1->number == n) {
				mem_peg_expunge(mt1);
				VTAILQ_REMOVE(&memtraces, mt1, list);
				free(mt1->lo);
				free(mt1->hi);
				free(mt1);
				return;
			}
		}
	} else {
		(void)cli_error(cli, USAGE);
	}
}
