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

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "r1000.h"
#include "ioc.h"
#include "vsb.h"

int optreset;		// Some have it, some not.

volatile nanosec simclock;
volatile nanosec systemc_t_zero;
volatile int systemc_clock;
unsigned do_trace;

static struct timespec t0;

#if !defined(WITH_SYSTEMC)
void v_matchproto_(cli_func_f)
cli_sc(struct cli *cli)
{
	cli_printf(cli, "SystemC not compiled in\n");
}
#endif

void
hexdump(struct vsb *vsb, const void *ptr, size_t len, unsigned offset)
{
	const uint8_t *p = ptr;
	size_t c, u, v;

	AN(ptr);
	for (u = 0; u < len; u += 0x20) {
		VSB_printf(vsb, "%08zx", u + offset);
		for (v = 0; v < 0x20 && v + u < len; v++)
			VSB_printf(vsb, " %02x", p[u + v]);
		for (; v < 0x20 && v + u < len; v++)
			VSB_cat(vsb, "   ");
		VSB_cat(vsb, " |");
		for (v = 0; v < 0x20 && v + u < len; v++) {
			c = p[u+v];
			if (c < 0x20 || c > 0x7e)
				c = '.';
			VSB_putc(vsb, c);
		}
		VSB_putc(vsb, '|');
		VSB_putc(vsb, '\n');
	}
}

void
finish(int status, const char *why)
{
	struct rusage rus;
	struct timespec tx;
	double ds, dr, dt;

	printf("Terminating because: %s\n", why);

	ds = simclock * 1e-9;
	printf("  %15.9f s simulated\n", ds);

	if (systemc_t_zero) {
		dt = ds - systemc_t_zero * 1e-9;
		printf("  %15.9f s SystemC simulation\n", dt);
	}

	AZ(clock_gettime(CLOCK_MONOTONIC, &tx));
	dr = 1e-9 * tx.tv_nsec;
	dr -= 1e-9 * t0.tv_nsec;
	dr += tx.tv_sec - t0.tv_sec;
	printf("  %12.6f s Wall Clock Time\n", dr);

	dt = 1e-9 * ioc_t_stopped;
	printf("  %12.6f s IOC stopped\n", dt);
	printf("  %ju IOC instructions\n", ioc_nins);

	if (dr > ds)
		printf("  1/%.2f Simulation ratio\n", dr / ds);
	else
		printf("  %.2f Simulation ratio\n", ds / dr);

	AZ(getrusage(RUSAGE_SELF, &rus));

	printf("  %5ld.%03ld s User time\n", rus.ru_utime.tv_sec, rus.ru_utime.tv_usec / 1000);
	printf("  %5ld.%03ld s System time\n", rus.ru_stime.tv_sec, rus.ru_stime.tv_usec / 1000);
	printf("  %ld Max RSS\n", rus.ru_maxrss);
	exit (status);
}

int
main(int argc, char **argv)
{
	int ch, i;
	long l;
	FILE *fi;

	AZ(clock_gettime(CLOCK_MONOTONIC, &t0));
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	diagbus_init();
	mem_init();
	ioc_init();

	while ((ch = getopt(argc, argv, "f:ht:T:")) != -1) {
		switch (ch) {
		case 'f':
			// handled in second pass
			break;
		case 't':
			l = strtoul(optarg, NULL, 0);
			if (l == 0)
				do_trace = 0;
			else if (l < 0)
				do_trace &= ~((unsigned long)(-l));
			else
				do_trace |= l;
			break;
		case 'T':
			trace_fd =
			    open(optarg, O_WRONLY|O_CREAT|O_TRUNC, 0644);
			if (trace_fd < 0) {
				fprintf(stderr, "Cannot open: %s: %s\n",
				    optarg, strerror(errno));
				exit(2);
			}
			break;
		default:
			fprintf(stderr, "Usage...\n");
			exit(0);
		}
	}

	optind = 1;
	optreset = 1;
	while ((ch = getopt(argc, argv, "f:ht:T:")) != -1) {
		switch(ch) {
		case 'f':
			fi = fopen(optarg, "r");
			if (fi == NULL) {
				fprintf(stderr, "Cannot open %s: %s\n",
				    optarg, strerror(errno));
				exit(2);
			}
			if (cli_from_file(fi, 1))
				exit(2);
			AZ(fclose(fi));
			break;
		default:
			break;
		}
	}

	argc -= optind;
	argv += optind;

	for (i = 0; i < argc; i++) {
		printf("CLI <%s>\n", argv[i]);
		if (cli_exec(argv[i]))
			exit(2);
	}

	printf("CLI open\n");

	(void)cli_from_file(stdin, 0);
	return (0);
}
