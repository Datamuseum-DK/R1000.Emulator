/*-
 * Copyright (c) 2005-2020 Poul-Henning Kamp
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

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>

#include "vqueue.h"

#define AZ(x) assert((x) == 0);
#define AN(x) assert((x) != 0);
#define WRONG() assert(__LINE__ == 0);

/*
 * In OO-light situations, functions have to match their prototype
 * even if that means not const'ing a const'able argument.
 * The typedef should be specified as argument to the macro.
 */
#define v_matchproto_(xxx)              /*lint --e{818} */

struct sim;
struct cli;

typedef int64_t			nanosec;

struct sim {

	pthread_mutex_t		run_mtx;
	pthread_mutex_t		running_mtx;
	pthread_cond_t		run_cond;
	pthread_cond_t		wait_cond;

	nanosec			simclock;

	uint64_t		pace_nsec;
	uint64_t		pace_n;
	uint64_t		ins_count;

	unsigned		do_trace;
	int			fd_trace;
};

extern struct sim *r1000sim;

/* CLI ****************************************************************/

struct cli {
	struct sim		*cs;
	int			status;
	int			help;
	int			ac;
	char			**av;
};

int cli_exec(struct sim *, const char *);
int cli_from_file(struct sim *cs, FILE *fi, int fatal);

typedef void cli_func_f(struct cli *);

void cli_printf(struct cli *cli, const char *fmt, ...) __printflike(2, 3);
int cli_error(struct cli *cli, const char *fmt, ...) __printflike(2, 3);

void cli_io_help(struct cli *, const char *desc, int trace, int elastic);

int cli_n_args(struct cli *cli, int n);
void cli_unknown(struct cli *cli);

/* Tracing & Debugging ************************************************/

void trace(int level, const char *fmt, ...) __printflike(2, 3);

/* UTILITIES  *********************************************************/

/* Safe printf into a fixed-size buffer */
#define bprintf(buf, fmt, ...)						\
	do {								\
		int ibprintf;						\
		ibprintf = snprintf(buf, sizeof buf, fmt, __VA_ARGS__);	\
		assert(ibprintf >= 0 && ibprintf < (int)sizeof buf);	\
	} while (0)
