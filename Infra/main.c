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

#include "r1000.h"
#include "elastic.h"
#include "ioc.h"
#include "vsb.h"

int optreset;		// Some have it, some not.

nanosec	simclock;
unsigned do_trace;
int fd_trace = -1;

void
trace(unsigned level, const char *fmt, ...)
{
	va_list ap;
	char buf[BUFSIZ];

	if (fd_trace < 0 || !(do_trace & level))
		return;
	bprintf(buf, "%12jd ", simclock);
	va_start(ap, fmt);
	vsnprintf(buf + 13, sizeof(buf) - 13, fmt, ap);
	va_end(ap);
	(void)write(fd_trace, buf, strlen(buf));
}

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
trace_dump(unsigned level, const void *ptr, size_t len, const char *fmt, ...)
{
	struct vsb *vsb;
	va_list ap;

	AN(ptr);
	if (fd_trace < 0 || !(do_trace & level))
		return;
	vsb = VSB_new_auto();
	AN(vsb);
	VSB_printf(vsb, "%12jd ", simclock);
	va_start(ap, fmt);
	VSB_vprintf(vsb, fmt, ap);
	va_end(ap);
	hexdump(vsb, ptr, len, 0);
	AZ(VSB_finish(vsb));
	VSB_tofile(vsb, fd_trace);
	VSB_destroy(&vsb);
}

int
main(int argc, char **argv)
{
	int ch, i;
	long l;
	FILE *fi;

	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	mem_init();
	ioc_init();
	i8052_init();	// After so diag_elastic is there

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
				do_trace &= ~(-l);
			else
				do_trace |= l;
			break;
		case 'T':
			fd_trace =
			    open(optarg, O_WRONLY|O_CREAT|O_TRUNC, 0644);
			if (fd_trace < 0) {
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
