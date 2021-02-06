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
#include "vav.h"
#include "ioc.h"

static int cli_alias_help(struct cli *cli, const char *canonical);

static void
cli_exit(struct cli *cli)
{
	uint16_t w;

	if (cli->help) {
		cli_printf(cli, "%s [<word>]\n", cli->av[0]);
		cli_printf(cli,
		    "\t\tExit emulator with optional return code\n");
		return;
	}
	printf("%ju instructions, %ju paces, %ju pace nsecs\n",
	    cli->cs->ins_count,
	    cli->cs->pace_n,
	    cli->cs->pace_nsec
	);
	if (cli->ac == 1)
		exit(0);
	if (cli_n_args(cli, 1))
		return;
	w = atoi(cli->av[1]);
	if (cli->status)
		exit(-1);
	exit(w);
}

/**********************************************************************/

void
cli_printf(struct cli *cli, const char *fmt, ...)
{
	va_list ap;

	(void)cli;

	va_start(ap, fmt);
	(void)vprintf(fmt, ap);
	va_end(ap);
}

int
cli_error(struct cli *cli, const char *fmt, ...)
{
	va_list ap;

	cli->status = 1;

	va_start(ap, fmt);
	(void)vprintf(fmt, ap);
	va_end(ap);
	return (1);
}

static int
cli_alias_help(struct cli *cli, const char *canonical)
{
	if (strcmp(cli->av[0], canonical)) {
		cli_printf(cli, "%s\t\tAlias for %s\n", cli->av[0], canonical);
		return (1);
	}
	return (0);
}

void
cli_io_help(struct cli *cli, const char *desc, int has_trace, int has_elastic)
{
	cli_printf(cli, "%s [<unit>] [arguments]\n", cli->av[0]);
	cli_printf(cli, "\t\t%s\n", desc);
	if (has_trace) {
		cli_printf(cli, "\ttrace <word>\n");
		cli_printf(cli, "\t\tI/O trace level.\n");
	}
	if (has_elastic) {
		cli_printf(cli, "\t<elastic>\n");
		cli_printf(cli, "\t\tElastic buffer arguments\n");
	}
}

void
cli_unknown(struct cli *cli)
{

	cli_printf(cli, "Unknown argument '%s'\n", cli->av[0]);
	cli->status = 1;
}

int
cli_n_args(struct cli *cli, int n)
{
	if (cli->ac == n + 1)
		return (0);
	if (n == 0)
		return (cli_error(cli, "Expected no arguments after '%s'\n",
		    cli->av[0]));
	return(cli_error(cli, "Expected %d arguments after '%s'\n",
	    n, cli->av[0]));
}

static cli_func_f cli_help;

static const struct cli_cmds {
	const char		*cmd;
	cli_func_f		*func;
} cli_cmds[] = {
	{ "help",		cli_help },
	{ "exit",		cli_exit },
	{ "?",			cli_help },
	{ "console",		cli_ioc_console },
	{ "reset",		cli_ioc_reset },
	{ NULL,			NULL },
};

static void v_matchproto_(cli_func_t)
cli_help(struct cli *cli)
{
	const struct cli_cmds *cc;
	char *save;
	int bug;

	if (cli->help) {
		if (cli_alias_help(cli, "help"))
			return;
		cli_printf(cli, "%s [<command>]\n", cli->av[0]);
		cli_printf(cli, "\t\tShow command syntax\n");
		return;
	}
	cli->help = 1;
	bug = cli->ac;
	for (cc = cli_cmds; cc->cmd != NULL; cc++) {
		if (cli->ac > 1 && strcmp(cli->av[1], cc->cmd))
			continue;
		save = cli->av[0];
		cli->av[0] = strdup(cc->cmd);
		AN(cli->av[0]);
		cc->func(cli);
		free(cli->av[0]);
		cli->av[0] = save;
		assert(bug == cli->ac);
		cli_printf(cli, "\n");
	}
	if (cli->ac == 1 || (cli->ac > 1 && !strcmp(cli->av[1], "elastic")))
		(void)cli_elastic(NULL, cli);
}

int
cli_exec(struct sim *cs, const char *s)
{
	int ac;
	char **av;
	const struct cli_cmds *cc;
	struct cli cli;

	av = VAV_Parse(s, &ac, ARGV_COMMENT);
	AN(av);
	if (av[0] != NULL) {
		printf("CLI error: %s\n", av[0]);
		VAV_Free(av);
		return (1);
	}
	if (av[1] == NULL) {
		VAV_Free(av);
		return (0);
	}
	for (cc = cli_cmds; cc->cmd != NULL; cc++)
		if (!strcasecmp(cc->cmd, av[1]))
			break;
	if (cc->cmd == NULL) {
		printf("CLI error: no command '%s'\n", av[1]);
		VAV_Free(av);
		return (1);
	}

	memset(&cli, 0, sizeof cli);
	cli.cs = cs;
	cli.ac = ac - 1;
	cli.av = av + 1;
	AN(cc->func);
	cc->func(&cli);
	VAV_Free(av);
	return (cli.status);
}

int
cli_from_file(struct sim *cs, FILE *fi, int fatal)
{
	char buf[BUFSIZ];
	char *p;
	int rv = 0;

	while (1) {
		if (fgets(buf, sizeof buf, fi) != buf)
			break;
		p = strchr(buf, '\n');
		if (p != NULL)
			*p = '\0';
		printf("cli <%s>\n", buf);
		rv = cli_exec(cs, buf);
		if (rv < 0 || (rv && fatal))
			break;
	}
	return (rv);
}
