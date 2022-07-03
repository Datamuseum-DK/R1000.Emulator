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

static struct memtrace *
get_next_trace(void)
{
	struct memtrace *mt;
	int n;

	mt = VTAILQ_LAST(&memtraces, memtracehead);
	if (mt != NULL)
		n = mt->number + 1;
	else
		n = 1;

	mt = calloc(sizeof *mt, 1);
	AN(mt);
	mt->number = n;
	return (mt);
}

#define USAGE_ADD() \
	do { \
		cli_usage(cli, "<segname>|<lo_adr> <hi_adr>", \
		    "Add a memory trace point."); \
		return; \
	} while (0)

static void v_matchproto_(cli_func_f)
cli_ioc_memtrace_add(struct cli *cli)
{
	struct memtrace *mt1;
	const struct memdesc *md;
	unsigned u, lo, hi;

	if (cli->help || cli->ac < 2 || cli->ac > 3)
		USAGE_ADD();

	cli->ac--;
	cli->av++;

	if (cli->ac == 1) {
		for (u = 0; u < n_memdesc; u++) {
			if (!strcmp(memdesc[u]->name, cli->av[0]))
				break;
		}
		if (u == n_memdesc) {
			cli_error(cli, "Unknown segment name\n");
			return;
		}
		mt1 = get_next_trace();
		md = memdesc[u];
		mt1->seg = md->name;
		mem_peg_register(md->lo, md->hi, memtrace_event, mt1);
		VTAILQ_INSERT_TAIL(&memtraces, mt1, list);
		return;
	} else {

		lo = strtoul(cli->av[0], NULL, 0);
		hi = strtoul(cli->av[1], NULL, 0);

		if (hi <= lo) {
			cli_error(cli, "hi_adr <= lo_adr\n");
			return;
		}

		mt1 = get_next_trace();
		mt1->lo = strdup(cli->av[0]);
		AN(mt1->lo);
		mt1->hi = strdup(cli->av[1]);
		AN(mt1->hi);

		mem_peg_register(lo, hi, memtrace_event, mt1);
		VTAILQ_INSERT_TAIL(&memtraces, mt1, list);
		return;
	}
}

static void v_matchproto_(cli_func_f)
cli_ioc_memtrace_del(struct cli *cli)
{
	struct memtrace *mt1;
	int n;

	if (cli->help || cli->ac != 2) {
		cli_usage(cli, "<number>", "Delete memtrace.");
		return;
	}
	cli->ac--;
	cli->av++;
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
	cli_error(cli, "memtrace %d not found\n", n);
}

static void v_matchproto_(cli_func_f)
cli_ioc_memtrace_list(struct cli *cli)
{
	struct memtrace *mt1;

	if (cli->help) {
		cli_usage(cli, NULL, "List memtrace.");
		return;
	}
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
			cli_printf(cli, " -hi %s", mt1->hi);
		cli_printf(cli, "\n");
	}
}

static const struct cli_cmds cli_ioc_memtrace_cmds[] = {
	{ "add",		cli_ioc_memtrace_add },
	{ "del",		cli_ioc_memtrace_del },
	{ "list",		cli_ioc_memtrace_list },
	{ NULL,			NULL },
};

void v_matchproto_(cli_func_f)
cli_ioc_memtrace(struct cli *cli)
{
	cli_redispatch(cli, cli_ioc_memtrace_cmds);
}
