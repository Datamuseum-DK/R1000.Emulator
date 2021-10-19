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

#ifndef FIRMWARE_PATH
#  define FIRMWARE_PATH "_FirmwareXXX"
#endif

#include "vqueue.h"

#define AZ(x) assert((x) == 0);
#define AN(x) assert((x) != 0);
#define WRONG() assert(__LINE__ == 0);

#ifndef __printflike
#    define __printflike(fmtarg, firstvararg) \
     __attribute__((__format__ (__printf__, fmtarg, firstvararg)))
#endif

#define TRUST_ME(ptr)  ((void*)(uintptr_t)(ptr))

struct vsb;

/**********************************************************************/
/*
 * In OO-light situations, functions have to match their prototype
 * even if that means not const'ing a const'able argument.
 * The typedef should be specified as argument to the macro.
 */
#define v_matchproto_(xxx)              /*lint --e{818} */

struct cli;
struct callout;

typedef int64_t			nanosec;
extern volatile nanosec		simclock;
extern volatile int		systemc_clock;
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

/* RPN evaluator ******************************************************/

struct rpn;

typedef void rpn_op_f(struct rpn *);

int Rpn_Eval(struct vsb *vsb, const char *pgm);
void Rpn_AddOp(const char *name, rpn_op_f *func);
int Rpn_Need(struct rpn *rpn, unsigned up, unsigned down);
intmax_t Rpn_Pop(struct rpn *rpn);
void Rpn_Push(struct rpn *rpn, intmax_t a);
int Rpn_Failed(const struct rpn *rpn);
void Rpn_Printf(const struct rpn *rpn, const char *fmt, ...);

#define RPN_POP(var)						\
	do {							\
		var = Rpn_Pop(rpn);				\
		if (Rpn_Failed(rpn))				\
			return;					\
	} while (0)

#define RPN_PUSH(val)						\
	do {							\
		Rpn_Push(rpn, val);				\
		if (Rpn_Failed(rpn))				\
			return;					\
	} while (0)

/* SystemC ************************************************************/

cli_func_f cli_sc;

/* Tracing & Debugging ************************************************/

void Trace(int flag, const char *fmt, ...) __printflike(2, 3);
void TraceDump(int flag, const void *ptr, size_t len,
    const char *fmt, ...) __printflike(4, 5);

#define trace(level, ...) \
	Trace(do_trace & level, __VA_ARGS__)

#define trace_dump(level, ptr, len, ...) \
	Trace(do_trace & level, ptr, len, __VA_ARGS__)

#include "trace.h"
#define TRACER(name, unused) extern int trace_##name;
TRACERS
#undef TRACER

cli_func_f cli_trace;

// Trace bits

#define TRACE_68K	(1<<0)		// Trace instructions
#define TRACE_IO	(1<<1)		// General I/O tracing
#define TRACE_SCSI	(1<<2)		// Trace SCSI-CDBs
#define TRACE_SCSI_DATA	(1<<3)		// Trace SCSI-CDBs
#define TRACE_PIT	(1<<4)		// PIT timer tracing
#define TRACE_SYSCALL	(1<<5)		// PIT timer tracing
#define TRACE_DIAG	(1<<6)		// DIAG bus

#define TRACE_ALL	((1LU<<32U)-1)	// Hvergang Perikles, Hvergang...

/* CALLOUTS ***********************************************************/

void callout_signal_cond(pthread_cond_t *cond,
    pthread_mutex_t *mtx, nanosec when, nanosec repeat);

typedef void callout_callback_f(void *);
void callout_callback(callout_callback_f *func,
    void *priv, nanosec when, nanosec repeat);
void callout_sleep(nanosec duration);

nanosec callout_poll(void);

/* DIAG *************************************************************/

extern struct elastic *diag_elastic;

cli_func_f cli_dummy_diproc;

/* MEMORY *************************************************************/

void mem_init(void);

/* UTILITIES **********************************************************/

void hexdump(struct vsb *vsb, const void *ptr, size_t len, unsigned offset);

/* Safe printf into a fixed-size buffer */
#define bprintf(buf, fmt, ...)						\
	do {								\
		int ibprintf;						\
		ibprintf = snprintf(buf, sizeof buf, fmt, __VA_ARGS__);	\
		assert(ibprintf >= 0 && ibprintf < (int)sizeof buf);	\
	} while (0)

