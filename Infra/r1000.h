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

#ifndef __printflike
#    define __printflike(fmtarg, firstvararg) \
     __attribute__((__format__ (__printf__, fmtarg, firstvararg)))
#endif

struct vsb;

// Trace bits

#define TRACE_68K	(1<<0)		// Trace instructions
#define TRACE_IO	(1<<1)		// General I/O tracing
#define TRACE_SCSI	(1<<2)		// Trace SCSI-CDBs
#define TRACE_SCSI_DATA	(1<<3)		// Trace SCSI-CDBs
#define TRACE_PIT	(1<<4)		// PIT timer tracing
#define TRACE_SYSCALL	(1<<5)		// PIT timer tracing
#define TRACE_DIAG	(1<<6)		// DIAG bus

#define TRACE_ALL	((1LU<<32U)-1)	// Hvergang Perikles, Hvergang...

/*
 * In OO-light situations, functions have to match their prototype
 * even if that means not const'ing a const'able argument.
 * The typedef should be specified as argument to the macro.
 */
#define v_matchproto_(xxx)              /*lint --e{818} */

struct cli;
struct callout;

typedef int64_t			nanosec;
extern nanosec			simclock;
extern unsigned			do_trace;
extern int			trace_fd;

/* CLI ****************************************************************/

struct cli {
	int			status;
	int			help;
	int			ac;
	char			**av;
	char			**av0;
};

typedef void cli_func_f(struct cli *);

struct cli_cmds {
	const char		*cmd;
	cli_func_f		*func;
};

void cli_path(struct cli *cli);
void cli_usage(struct cli *cli, const char *fmt, ...) __printflike(2,3);
void cli_dispatch(struct cli *cli, const struct cli_cmds *cmds);

int cli_exec(const char *);
int cli_from_file(FILE *fi, int fatal);


void cli_printf(struct cli *cli, const char *fmt, ...) __printflike(2, 3);
int cli_error(struct cli *cli, const char *fmt, ...) __printflike(2, 3);

void cli_io_help(struct cli *, const char *desc, int trace, int elastic);

int cli_n_m_args(struct cli *cli, int minarg, int maxarg,
    const char *fmt, ...) __printflike(4, 5);
int cli_n_args(struct cli *cli, int maxarg);
void cli_unknown(struct cli *cli);

/* Tracing & Debugging ************************************************/

void hexdump(struct vsb *vsb, const void *ptr, size_t len, unsigned offset);
void trace(unsigned level, const char *fmt, ...) __printflike(2, 3);
void trace_dump(unsigned level, const void *ptr, size_t len,
    const char *fmt, ...) __printflike(4, 5);


/* CALLOUTS ***********************************************************/

void callout_signal_cond(pthread_cond_t *cond,
    pthread_mutex_t *mtx, nanosec when, nanosec repeat);

typedef void callout_callback_f(void *);
void callout_callback(callout_callback_f *func,
    void *priv, nanosec when, nanosec repeat);

nanosec callout_poll(void);

/* DIAG *************************************************************/

extern struct elastic *diag_elastic;

void i8052_init(void);

/* MEMORY *************************************************************/

void mem_init(void);

/* UTILITIES **********************************************************/

/* Safe printf into a fixed-size buffer */
#define bprintf(buf, fmt, ...)						\
	do {								\
		int ibprintf;						\
		ibprintf = snprintf(buf, sizeof buf, fmt, __VA_ARGS__);	\
		assert(ibprintf >= 0 && ibprintf < (int)sizeof buf);	\
	} while (0)
