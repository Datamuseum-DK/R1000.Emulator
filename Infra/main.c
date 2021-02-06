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

int optreset;		// Some have it, some not.

struct sim *r1000sim;

static struct sim *
sim_new(void)
{
	struct sim *cs;

	cs = calloc(1, sizeof *cs);
	AN(cs);

	AZ(pthread_mutex_init(&cs->run_mtx, NULL));
	AZ(pthread_mutex_init(&cs->running_mtx, NULL));
	AZ(pthread_cond_init(&cs->run_cond, NULL));
	AZ(pthread_cond_init(&cs->wait_cond, NULL));
	AZ(pthread_mutex_init(&cs->callout_mtx, NULL));

	TAILQ_INIT(&cs->callouts);
	cs->fd_trace = -1;

	return (cs);
}

void
trace(int level, const char *fmt, ...)
{
	va_list ap;
	struct sim *cs = r1000sim;
	char buf[BUFSIZ];

	AN(cs);
	if (cs->fd_trace < 0 || !(cs->do_trace & level))
		return;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof buf, fmt, ap);
	va_end(ap);
	(void)write(cs->fd_trace, buf, strlen(buf));
}

int
main(int argc, char **argv)
{
	int ch, i;
	int bare = 0;
	struct sim *cs = NULL;
	long l;
	FILE *fi;

	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	r1000sim = sim_new();
	cs = r1000sim;

	ioc_console_init(cs);
	ioc_duart_init(cs);

	while ((ch = getopt(argc, argv, "b:f:ht:T:")) != -1) {
		switch (ch) {
		case 'b':
			bare = 1;
			break;
		case 'f':
			// handled in second pass
			break;
		case 't':
			l = strtoul(optarg, NULL, 0);
			if (l == 0)
				cs->do_trace = 0;
			else if (l < 0)
				cs->do_trace &= ~(-l);
			else
				cs->do_trace |= l;
			break;
		case 'T':
			cs->fd_trace =
			    open(optarg, O_WRONLY|O_CREAT|O_TRUNC, 0644);
			if (cs->fd_trace < 0) {
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
	while ((ch = getopt(argc, argv, "b:f:ht:T:")) != -1) {
		switch(ch) {
		case 'f':
			fi = fopen(optarg, "r");
			if (fi == NULL) {
				fprintf(stderr, "Cannot open %s: %s\n",
				    optarg, strerror(errno));
				exit(2);
			}
			if (cli_from_file(cs, fi, 1))
				exit(2);
			fclose(fi);
			break;
		default:
			break;
		}
	}

	argc -= optind;
	argv += optind;

	for (i = 0; i < argc; i++) {
		printf("CLI <%s>\n", argv[i]);
		if (cli_exec(cs, argv[i]))
			exit(2);
	}

	printf("CLI open\n");

	(void)cli_from_file(cs, stdin, 0);
	return (0);
}
