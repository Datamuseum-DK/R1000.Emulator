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

#include <fcntl.h>
#include <inttypes.h>
#include <pthread.h>
#include <regex.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "r1000.h"
#include "vqueue.h"

#include "r1000sc.h"
#include "r1000sc_priv.h"

struct component {
	VTAILQ_ENTRY(component)	list;
	char			*name;
	int			*flags;
};

int sc_boards;

static VTAILQ_HEAD(,component) component_list =
    VTAILQ_HEAD_INITIALIZER(component_list);

static int ncomponents = 0;
static pthread_t sc_runner;
static pthread_mutex_t sc_mtx;
static pthread_cond_t sc_cond;
static double sc_quota = 0;
static int sc_quota_exit = 0;
static int sc_started;
static struct timespec sc_t0;

void
sysc_trace(const char *me, const char *fmt)
{
	Trace(trace_systemc, "SC %.0f %s %s", sc_now(), me, fmt);
}

void
sc_tracef(const char *me, const char *fmt, ...)
{
	va_list ap;
	char buf[BUFSIZ];

	bprintf(buf, "SC %.0f %s ", sc_now(), me);
	va_start(ap, fmt);
	Tracev(trace_systemc, buf, fmt, ap);
	va_end(ap);
}


void
should_i_trace(const char *me, uint32_t *p)
{
	struct component *comp;

	AN(me);
	AN(p);
	comp = calloc(sizeof *comp, 1);
	AN(comp)
	comp->name = strdup(me);
	AN(comp->name)
	comp->flags = p;
	VTAILQ_INSERT_TAIL(&component_list, comp, list);
	ncomponents++;

	*p = 0;
	return;
}

void
load_programmable(const char *who, void *dst, size_t size, const char *arg)
{
	char buf[BUFSIZ];
	FILE *f;
	size_t sz;

	bprintf(buf, "%s/%s.bin", FIRMWARE_PATH, arg);
	f = fopen(buf, "r");
	if (f == NULL) {
		bprintf(buf, "%s/%s-01.bin", FIRMWARE_PATH, arg);
		f = fopen(buf, "r");
	}
	if (f == NULL) {
		bprintf(buf, "%s/%s-02.bin", FIRMWARE_PATH, arg);
		f = fopen(buf, "r");
	}
	if (f == NULL) {
		bprintf(buf, "%s/%s-03.bin", FIRMWARE_PATH, arg);
		f = fopen(buf, "r");
	}
	if (f == NULL) {
		bprintf(buf, "%s/PROM-%s.bin", FIRMWARE_PATH, arg);
		f = fopen(buf, "r");
	}
	if (f == NULL) {
		fprintf(stderr, "Firmware '%s' missing for '%s'\n", arg, who);
		fprintf(stderr, "(run 'make setup_systemc' ?)\n");
		exit(2);
	}
	AN(f);
	sc_tracef(who, "FIRMWARE %s => %s", arg, buf);
	sz = fread(dst, 1, size, f);
	assert(sz == size);
	AZ(fclose(f));
}

double
sc_main_get_quota(void)
{
	double retval;

	AZ(pthread_mutex_lock(&sc_mtx));
	if (sc_started == 1) {
		AZ(pthread_cond_signal(&sc_cond));
		sc_started = 2;
	}
	retval = sc_quota;
	while (retval <= 0 && (sc_started < 4 || !sc_quota_exit)) {
		AZ(pthread_cond_wait(&sc_cond, &sc_mtx));
		retval = sc_quota;
	}
	sc_quota -= retval;
	AZ(pthread_mutex_unlock(&sc_mtx));
	if (sc_quota_exit && retval == 0) {
		fprintf(stderr, "QQQ sc_started %d retval %g %a\n", sc_started, retval, retval);
		finish(1, "SystemC quota exhausted");
	}
	assert(retval > 0);
	return (retval);
}

static void v_matchproto_(cli_func_f)
cli_sc_launch(struct cli *cli)
{
	int i;

	if (cli->help) {
		cli_printf(cli, "Usage:\n\t[boardname|all]...\n");
		return;
	}
	sc_boards |= R1K_BOARD_IOC;
	for (i = 1; i < cli->ac; i++) {
		if (!strcmp(cli->av[i], "all")) {
			sc_boards |= R1K_BOARD_ALL;
		} else if (!strcmp(cli->av[i], "ioc"))
			sc_boards |= R1K_BOARD_IOC;
		else if (!strcmp(cli->av[i], "val"))
			sc_boards |= R1K_BOARD_VAL;
		else if (!strcmp(cli->av[i], "typ"))
			sc_boards |= R1K_BOARD_TYP;
		else if (!strcmp(cli->av[i], "seq"))
			sc_boards |= R1K_BOARD_SEQ;
		else if (!strcmp(cli->av[i], "fiu"))
			sc_boards |= R1K_BOARD_FIU;
		else if (!strcmp(cli->av[i], "mem0"))
			sc_boards |= R1K_BOARD_MEM32_0;
		else if (!strcmp(cli->av[i], "mem2"))
			sc_boards |= R1K_BOARD_MEM32_2;
		else if (!strcmp(cli->av[i], "mem32")) {
			if (sc_boards & R1K_BOARD_MEM32_0)
				sc_boards |= R1K_BOARD_MEM32_2;
			else
				sc_boards |= R1K_BOARD_MEM32_0;
		} else {
			cli_error(cli, "Bad board name `%s`\n", cli->av[i]);
		}
	}

	systemc_clock = 1;

	AZ(pthread_mutex_init(&sc_mtx, NULL));
	AZ(pthread_cond_init(&sc_cond, NULL));

	sc_started = 1;

	AZ(pthread_create(&sc_runner, NULL, sc_main_thread, NULL));

	AZ(pthread_mutex_lock(&sc_mtx));
	while (sc_started < 2)
		AZ(pthread_cond_wait(&sc_cond, &sc_mtx));

	AZ(clock_gettime(CLOCK_REALTIME, &sc_t0));
	systemc_t_zero = simclock;
	sc_started = 3;
	AZ(pthread_mutex_unlock(&sc_mtx));

	cli_printf(cli, "SystemC started, %d components\n", ncomponents);
}

static void v_matchproto_(cli_func_f)
cli_sc_quota(struct cli *cli)
{

	if (cli->help) {
		cli_printf(cli, "Usage:\n\t\"wait\"|microseconds\n");
		return;
	}
	AZ(pthread_mutex_lock(&sc_mtx));
	if (cli->ac == 2 && !strcmp(cli->av[1], "exit")) {
		sc_quota_exit = 1;
	} else if (cli->ac == 2 && !strcmp(cli->av[1], "wait")) {
		while (sc_quota > 0) {
			AZ(pthread_mutex_unlock(&sc_mtx));
			usleep(100000);
			AZ(pthread_mutex_lock(&sc_mtx));
		}
	} else {
		sc_quota += strtod(cli->av[1], NULL);
		sc_started = 4;
	}
	cli_printf(cli, "SC_QUOTA = %.9f\n", sc_quota);
	AZ(pthread_cond_signal(&sc_cond));
	AZ(pthread_mutex_unlock(&sc_mtx));
}

static void v_matchproto_(cli_func_f)
cli_sc_wait(struct cli *cli)
{
	double e = 0.01;

	if (cli->ac > 1)
		e = strtod(cli->av[1], NULL);
	do
		usleep(10000);
	while (e < sc_when());
}

static void v_matchproto_(cli_func_f)
cli_sc_rate(struct cli *cli)
{
	struct timespec t1;
	double d, e;

	AZ(clock_gettime(CLOCK_REALTIME, &t1));
	e = sc_when();
	d = 1e-9 * (t1.tv_nsec - sc_t0.tv_nsec);
	d += (t1.tv_sec - sc_t0.tv_sec);
	cli_printf(cli, "SC real time: %.3f\tsim time: %.3f\tratio: %.3f\n", d, e, d / e);
}

#define TRACE_USAGE "Usage:\n\t[regexp ['on'|'off']]\n"

static void v_matchproto_(cli_func_f)
cli_sc_trace(struct cli *cli)
{
	struct component *comp;
	regex_t rex;
	char errbuf[BUFSIZ];
	const char *regexp;
	int onoff = 0;
	int err;

	if (cli->help) {
		cli_printf(cli, TRACE_USAGE);
		return;
	}
	if (cli->ac < 2)
		regexp = ".";
	else
		regexp = cli->av[1];

	if (cli->ac == 3 && !strcmp(cli->av[2], "on"))
		onoff = 1;
	else if (cli->ac == 3 && !strcmp(cli->av[2], "off"))
		onoff = 0;
	else if (cli->ac == 3)
		onoff = strtoul(cli->av[2], NULL, 0);

	err = regcomp(&rex, regexp, 0);
	if (err) {
		(void)regerror(err, &rex, errbuf, sizeof errbuf);
		cli_error(cli, "Regexp error: %s\n", errbuf);
		return;
	}

	VTAILQ_FOREACH(comp, &component_list, list) {
		if (!regexec(&rex, comp->name, 0, 0, 0)) {
			*comp->flags = onoff;
			cli_printf(
			    cli,
			    "    0x%x %s\n", *comp->flags, comp->name
			);
		}
	}
	regfree(&rex);
}

static const struct cli_cmds cli_sc_cmds[] = {
	{ "launch",		cli_sc_launch },
	{ "q",			cli_sc_quota },
	{ "quota",		cli_sc_quota },
	{ "rate",		cli_sc_rate },
	{ "trace",		cli_sc_trace },
	{ "wait",		cli_sc_wait },
	{ NULL,			NULL },
};

void v_matchproto_(cli_func_f)
cli_sc(struct cli *cli)
{
	if (cli->ac > 1 || cli->help) {
		cli->ac--;
		cli->av++;
		cli_dispatch(cli, cli_sc_cmds);
	}
}

