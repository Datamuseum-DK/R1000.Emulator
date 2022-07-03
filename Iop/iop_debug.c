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

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Infra/r1000.h"
#include "Musashi/m68k.h"
#include "Ioc/ioc.h"
#include "Infra/vsb.h"
#include "Ioc/memspace.h"

static struct vsb *debug_vsb;
static pthread_mutex_t bpt_mtx = PTHREAD_MUTEX_INITIALIZER;

struct breakpoint {
	uint32_t			adr;
	ioc_bpt_f			*func;
	void				*priv;
	VTAILQ_ENTRY(breakpoint)	list;
};

static VTAILQ_HEAD(, breakpoint) breakpoints =
    VTAILQ_HEAD_INITIALIZER(breakpoints);

void
ioc_dump_cpu_regs(struct vsb *vsb)
{
	VSB_printf(vsb, "PC = 0x%x", m68k_get_reg(NULL, M68K_REG_PC));
	VSB_printf(vsb, "  SR = 0x%x\n", m68k_get_reg(NULL, M68K_REG_SR));
	VSB_printf(vsb, "D0 = %08x  D4 = %08x   A0 = %08x  A4 = %08x\n",
	    m68k_get_reg(NULL, M68K_REG_D0), m68k_get_reg(NULL, M68K_REG_D4),
	    m68k_get_reg(NULL, M68K_REG_A0), m68k_get_reg(NULL, M68K_REG_A4)
	);

	VSB_printf(vsb, "D1 = %08x  D5 = %08x   A1 = %08x  A5 = %08x\n",
	    m68k_get_reg(NULL, M68K_REG_D1), m68k_get_reg(NULL, M68K_REG_D5),
	    m68k_get_reg(NULL, M68K_REG_A1), m68k_get_reg(NULL, M68K_REG_A5)
	);

	VSB_printf(vsb, "D2 = %08x  D6 = %08x   A2 = %08x  A6 = %08x\n",
	    m68k_get_reg(NULL, M68K_REG_D2), m68k_get_reg(NULL, M68K_REG_D6),
	    m68k_get_reg(NULL, M68K_REG_A2), m68k_get_reg(NULL, M68K_REG_A6)
	);

	VSB_printf(vsb, "D3 = %08x  D7 = %08x   A3 = %08x  A7 = %08x\n",
	    m68k_get_reg(NULL, M68K_REG_D3), m68k_get_reg(NULL, M68K_REG_D7),
	    m68k_get_reg(NULL, M68K_REG_A3), m68k_get_reg(NULL, M68K_REG_A7)
	);

}

void
ioc_dump_core(const char *fn)
{
	FILE *f;

	AN(fn);
	f = fopen(fn, "w");
	assert (f != NULL);
	(void)fwrite(ram_space, sizeof(ram_space), 1, f);
	(void)fclose(f);
}

void v_matchproto_(cli_func_f)
cli_ioc_dump(struct cli *cli)
{
	const char *fn;

	if (cli->help) {
		cli_usage(cli, NULL, "Dump IOC RAM to file.");
		return;
	}
	cli->ac--;
	cli->av++;
	if (cli_n_m_args(cli, 0, 1, ""))
		return;
	if (cli->ac == 0)
		fn = "/tmp/_ioc.ram";
	else
		fn = cli->av[0];
	ioc_dump_core(fn);
	cli_printf(cli, "Core dumped to '%s'\n", fn);
}

/**********************************************************************
 */

static int v_matchproto_(ioc_bpt_f)
ioc_breakpoint_rpn_func(void *priv, uint32_t adr)
{
	int i;

	AN(debug_vsb);
	VSB_clear(debug_vsb);
	i = Rpn_Eval(debug_vsb, (const char *)priv);
	AZ(VSB_finish(debug_vsb));
	if (VSB_len(debug_vsb))
		Trace(1, "Breakpoint %08x %s", adr, VSB_data(debug_vsb));
	if (i)
		printf("\nBad RPN in breakpoint 0x%08x: %s\n",
		    adr, (const char *)priv);
	return (0);
}

void
ioc_breakpoint_check(uint32_t adr)
{
	struct breakpoint *bp, *bp2;
	int did = 0, i;

	AZ(pthread_mutex_lock(&bpt_mtx));
	VTAILQ_FOREACH_SAFE(bp, &breakpoints, list, bp2) {
		if (bp->adr == adr) {
			did = 1;
			AZ(pthread_mutex_unlock(&bpt_mtx));
			i = bp->func(bp->priv, bp->adr);
			AZ(pthread_mutex_lock(&bpt_mtx));
			VTAILQ_REMOVE(&breakpoints, bp, list);
			if (i)
				free(bp);
			else
				VTAILQ_INSERT_HEAD(&breakpoints, bp, list);
		}
	}
	AZ(pthread_mutex_unlock(&bpt_mtx));
	if (!did)
		mem_peg_reset(adr, adr + 2, PEG_BREAKPOINT);
}

void
ioc_breakpoint(uint32_t adr, ioc_bpt_f *func, void *priv)
{
	struct breakpoint *bp;

	AN(func);
	bp = calloc(sizeof *bp, 1);
	AN(bp);
	bp->adr = adr;
	bp->func = func;
	bp->priv = priv;
	AZ(pthread_mutex_lock(&bpt_mtx));
	VTAILQ_INSERT_TAIL(&breakpoints, bp, list);
	AZ(pthread_mutex_unlock(&bpt_mtx));
	mem_peg_set(adr, adr + 2, PEG_BREAKPOINT);
}

void
ioc_breakpoint_rpn(uint32_t adr, const char *rpn)
{
	char *p;

	p = strdup(rpn);
	AN(p);
	ioc_breakpoint(adr, ioc_breakpoint_rpn_func, p);
}

void v_matchproto_(cli_func_f)
cli_ioc_breakpoint(struct cli *cli)
{
	uint32_t adr;

	if (cli->help) {
		cli_usage(cli, "<address> <rpn>", "Set Breakpoint.");
		return;
	}
	cli->ac--;
	cli->av++;
	if (cli_n_m_args(cli, 2, 2, ""))
		return;
	adr = strtoul(cli->av[0], NULL, 0);
	ioc_breakpoint_rpn(adr, cli->av[1]);
}

/**********************************************************************
 * RPN operators
 */

#define RPN_REGS \
	RPN_REG(PC) \
	RPN_REG(SR) \
	RPN_REG(A0) \
	RPN_REG(A1) \
	RPN_REG(A2) \
	RPN_REG(A3) \
	RPN_REG(A4) \
	RPN_REG(A5) \
	RPN_REG(A6) \
	RPN_REG(A7) \
	RPN_REG(D0) \
	RPN_REG(D1) \
	RPN_REG(D2) \
	RPN_REG(D3) \
	RPN_REG(D4) \
	RPN_REG(D5) \
	RPN_REG(D6) \
	RPN_REG(D7)

#define RPN_REG(reg)						\
	static void v_matchproto_(rpn_op_f)			\
	rpn_##reg(struct rpn *rpn)				\
	{							\
		RPN_PUSH(m68k_get_reg(NULL, M68K_REG_##reg));	\
	}							\
	static void v_matchproto_(rpn_op_f)			\
	rpn_dot_##reg(struct rpn *rpn)				\
	{							\
		Rpn_Printf(rpn, #reg "=%08x",			\
		m68k_get_reg(NULL, M68K_REG_##reg));		\
	}							\
	static void v_matchproto_(rpn_op_f)			\
	rpn_bang_##reg(struct rpn *rpn)				\
	{							\
		unsigned u;					\
		RPN_POP(u);					\
		m68k_set_reg(M68K_REG_##reg, u);		\
	}

RPN_REGS
#undef RPN_REG

static void v_matchproto_(rpn_op_f)
rpn_sp_0(struct rpn *rpn)
{
	intmax_t a7;
	a7 = m68k_get_reg(NULL, M68K_REG_A7);
	RPN_PUSH(a7);
}

#define RPN_SPS \
	RPN_SP(1) \
	RPN_SP(2) \
	RPN_SP(3) \
	RPN_SP(4) \
	RPN_SP(5) \
	RPN_SP(6) \
	RPN_SP(7) \
	RPN_SP(8) \
	RPN_SP(9) \
	RPN_SP(10) \
	RPN_SP(11) \
	RPN_SP(12)

#define RPN_SP(nbr)						\
	static void v_matchproto_(rpn_op_f)			\
	rpn_sp_##nbr(struct rpn *rpn)				\
	{							\
		intmax_t a7;					\
		a7 = m68k_get_reg(NULL, M68K_REG_A7);		\
		RPN_PUSH(a7 + 2U * nbr);			\
	}

RPN_SPS
#undef RPN_SP

static void v_matchproto_(rpn_op_f)
rpn_load_b(struct rpn *rpn)
{
	intmax_t a, b;

	RPN_POP(a);
	b = m68k_debug_read_memory_8((unsigned)a);
	RPN_PUSH(b);
}

static void v_matchproto_(rpn_op_f)
rpn_load_w(struct rpn *rpn)
{
	intmax_t a, b;

	RPN_POP(a);
	b = m68k_debug_read_memory_16((unsigned)a);
	RPN_PUSH(b);
}

static void v_matchproto_(rpn_op_f)
rpn_load_l(struct rpn *rpn)
{
	intmax_t a, b;

	RPN_POP(a);
	b = m68k_debug_read_memory_32((unsigned)a);
	RPN_PUSH(b);
}

static void v_matchproto_(rpn_op_f)
rpn_comma(struct rpn *rpn)
{

	Rpn_Printf(rpn, ", ");
}

static void v_matchproto_(rpn_op_f)
rpn_string(struct rpn *rpn)
{
	intmax_t a;
	unsigned u, v;
	uint8_t c;

	RPN_POP(a);
	u = m68k_debug_read_memory_32((unsigned)a);
	if (u == 1) {
		Rpn_Printf(rpn, "NullString");
		return;
	}
	v = m68k_debug_read_memory_32(u - 4);
	if (v != u) {
		Rpn_Printf(rpn, "BadString(%08x vs %08x)", u, v);
		return;
	}
	v = m68k_debug_read_memory_16(u);
	if (v > 0x7d) {
		Rpn_Printf(rpn, "BadString(len %04x)", v);
		return;
	}
	u += 2;
	Rpn_Printf(rpn, "'");
	while (v--) {
		c = m68k_debug_read_memory_8(u++);
		if (0x20 <= c && c <= 0x7e)
			Rpn_Printf(rpn, "%c", c);
		else
			Rpn_Printf(rpn, "\\x%02x", c);
	}
	Rpn_Printf(rpn, "'");
}

static void v_matchproto_(rpn_op_f)
rpn_dirent(struct rpn *rpn)
{
	intmax_t a;
	unsigned u, v, w;
	uint8_t c;

	RPN_POP(a);
	u = m68k_debug_read_memory_32((unsigned)a);
	// u = m68k_debug_read_memory_32(u);
	Rpn_Printf(rpn, "Dirent@0x%08x{'", u);
	for (v = 0; v < 0x1c; v++) {
		c = m68k_debug_read_memory_8(u++);
		if (!c)
			break;
		if (0x20 <= c && c <= 0x7e)
			Rpn_Printf(rpn, "%c", c);
		else
			Rpn_Printf(rpn, "\\x%02x", c);
	}
	Rpn_Printf(rpn, "', ");
	for (; v < 0x1c; v++) {
		c = m68k_debug_read_memory_8(u++);
		if (c)
			Rpn_Printf(rpn, " [Bogo @%x: x%02x]", v, c);
	}
	for (; v < 0x23; v++) {
		c = m68k_debug_read_memory_8(u++);
		Rpn_Printf(rpn, "%02x", c);
	}
	w = m68k_debug_read_memory_16(u);
	Rpn_Printf(rpn, ", lba=0x%04x, ", w);
	v += 2;
	u += 2;
	for (; v < 0x39; v++) {
		c = m68k_debug_read_memory_8(u++);
		if (c)
			Rpn_Printf(rpn, " [Bogo @%x: x%02x]", v, c);
	}
	for (; v < 0x40; v++) {
		c = m68k_debug_read_memory_8(u++);
		Rpn_Printf(rpn, "%02x", c);
	}
	Rpn_Printf(rpn, "}");
}

static void v_matchproto_(rpn_op_f)
rpn_hexdump(struct rpn *rpn)
{
	unsigned u;
	const char *sep = "[";
	intmax_t a, b;

	RPN_POP(a);
	RPN_POP(b);
	for (u = 0; u < a; u++) {
		Rpn_Printf(rpn, "%s%02x",
		    sep, m68k_debug_read_memory_8(b + u));
		sep = " ";
	}
	if (!a)
		Rpn_Printf(rpn, "%s", sep);
	Rpn_Printf(rpn, "]");
}

static void v_matchproto_(rpn_op_f)
rpn_ascii(struct rpn *rpn)
{
	unsigned u, c;
	const char *sep = "\"";
	intmax_t a, b;

	RPN_POP(a);
	RPN_POP(b);
	for (u = 0; u < a; u++) {
		c = m68k_debug_read_memory_8(b + u);
		if (0x20 <= c && c <= 0x7e)
			Rpn_Printf(rpn, "%s%c", sep, c);
		else
			Rpn_Printf(rpn, "%s\\x%02x", sep, c);
		sep = "";
	}
	if (!a)
		Rpn_Printf(rpn, "%s", sep);
	Rpn_Printf(rpn, "\"");
}

static void v_matchproto_(rpn_op_f)
rpn_stack(struct rpn *rpn)
{
	unsigned a7, u;
	const char *sep = "[";

	a7 = m68k_get_reg(NULL, M68K_REG_A7);
	for (u = 0; u < 32; u += 2) {
		Rpn_Printf(rpn, "%s%04x",
		    sep, m68k_debug_read_memory_16(a7 + u));
		sep = " ";
	}
	Rpn_Printf(rpn, "]");
}

static void v_matchproto_(rpn_op_f)
rpn_regs(struct rpn *rpn)
{
	const char *sep = "{";
#define RPN_REG(reg) \
	Rpn_Printf(rpn, "%s" #reg "=%08x", sep, m68k_get_reg(NULL, M68K_REG_##reg)); \
	sep = ", ";
RPN_REGS
#undef RPN_REG
	Rpn_Printf(rpn, "}");
}

static void v_matchproto_(rpn_op_f)
rpn_stop(struct rpn *rpn)
{
	ioc_stop_cpu();
	Rpn_Printf(rpn, "IOC CPU Stopped");
	printf("\nIOC CPU stopped by breakpoint\n");
}

void
ioc_debug_init(void)
{
	debug_vsb = VSB_new_auto();
	AN(debug_vsb);

#define RPN_REG(reg)				\
	Rpn_AddOp(#reg, rpn_##reg);		\
	Rpn_AddOp("." #reg, rpn_dot_##reg);	\
	Rpn_AddOp("!" #reg, rpn_bang_##reg);

	RPN_REGS
#undef RPN_REG
	Rpn_AddOp("sp", rpn_sp_0);
	Rpn_AddOp("sp+0", rpn_sp_0);
#define RPN_SP(nbr)	Rpn_AddOp("sp+" #nbr, rpn_sp_##nbr);
	RPN_SPS
#undef RPN_SP
	Rpn_AddOp("@B", rpn_load_b);
	Rpn_AddOp("@W", rpn_load_w);
	Rpn_AddOp("@L", rpn_load_l);
	Rpn_AddOp(",", rpn_comma);
	Rpn_AddOp("String", rpn_string);
	Rpn_AddOp("Dirent", rpn_dirent);
	Rpn_AddOp("stack", rpn_stack);
	Rpn_AddOp("regs", rpn_regs);
	Rpn_AddOp("stop", rpn_stop);
	Rpn_AddOp("hexdump", rpn_hexdump);
	Rpn_AddOp("ascii", rpn_ascii);
}
