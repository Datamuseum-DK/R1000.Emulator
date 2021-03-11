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

#include <fnmatch.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "vsb.h"

#include "r1000.h"

int trace_fd = -1;
static struct vsb *trace_vsb;
static pthread_mutex_t trace_mtx = PTHREAD_MUTEX_INITIALIZER;

/**********************************************************************/

#define TRACER(name, unused) int trace_##name;
TRACERS
#undef TRACER

static struct trace_spec {
	const char *name;
	const char *help;
	int *flag;
} traces[] = {
#define TRACER(name, help) { #name, help, &trace_##name },
TRACERS
#undef TRACER
	{ NULL, NULL, NULL }
};

/**********************************************************************/

typedef void modfunc(int *);

static void mod_set(int *ptr) { *(ptr) = 1; }
static void mod_clr(int *ptr) { *(ptr) = 0; }
static void mod_inv(int *ptr) { *(ptr) ^= 1; }
static void mod_nop(int *ptr) { (void)ptr; }

void v_matchproto_(cli_func_f)
cli_trace(struct cli *cli)
{
	struct trace_spec *tp;
	const char *p;
	modfunc *fp;
	int i;

	if (cli->help) {
		for (tp = traces; tp->name; tp++) {
			if (cli->ac > 1 &&
			    fnmatch(cli->av[1], tp->name, 0))
				continue;
			cli_printf(cli, "\t%s - %s\n",
			    tp->name, tp->help);
		}
		return;
	}
	if (cli->ac == 1) {
		for (tp = traces; tp->name; tp++)
			cli_printf(cli, "%s is %s\n",
			    tp->name, *(tp->flag) ? "on" : "off");
		return;
	}
	while (--cli->ac) {
		cli->av++;
		p = cli->av[0];
		switch (*p) {
		case '-': fp = mod_clr; p++; break;
		case '+': fp = mod_set; p++; break;
		case '~': fp = mod_inv; p++; break;
		case '!': fp = mod_inv; p++; break;
		default: fp = mod_nop; break;
		}
		for (tp = traces; tp->name; tp++) {
			if (fnmatch(p, tp->name, 0))
				continue;
			i = *(tp->flag);
			fp(tp->flag);
			if (i != *(tp->flag) || fp == mod_nop)
				cli_printf(cli, "%s is %s\n",
				    tp->name, *(tp->flag) ? "on" : "off");
		}
	}
}

/**********************************************************************/

static void
trace_init(void)
{
	if (trace_vsb == NULL) {
		trace_vsb = VSB_new_auto();
		AN(trace_vsb);
	} else {
		VSB_clear(trace_vsb);
	}
}

void
Trace(int flag, const char *fmt, ...)
{
	va_list ap;

	if (!flag || trace_fd < 0)
		return;

	AZ(pthread_mutex_lock(&trace_mtx));
	trace_init();

	VSB_printf(trace_vsb, "%12jd ", simclock);
	va_start(ap, fmt);
	VSB_vprintf(trace_vsb, fmt, ap);
	va_end(ap);
	VSB_putc(trace_vsb, '\n');
	AZ(VSB_finish(trace_vsb));
	(void)VSB_tofile(trace_vsb, trace_fd);
	AZ(pthread_mutex_unlock(&trace_mtx));
}

void
TraceDump(int flag, const void *ptr, size_t len, const char *fmt, ...)
{
	va_list ap;

	AN(ptr);
	if (!flag || trace_fd < 0)
		return;
	AZ(pthread_mutex_lock(&trace_mtx));
	trace_init();

	VSB_printf(trace_vsb, "%12jd ", simclock);
	va_start(ap, fmt);
	VSB_vprintf(trace_vsb, fmt, ap);
	va_end(ap);
	hexdump(trace_vsb, ptr, len, 0);
	AZ(VSB_finish(trace_vsb));
	VSB_tofile(trace_vsb, trace_fd);
	AZ(pthread_mutex_unlock(&trace_mtx));
}