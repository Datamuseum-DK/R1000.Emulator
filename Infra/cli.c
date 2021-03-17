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
#include "vav.h"
#include "ioc.h"

static void
cli_exit(struct cli *cli)
{
	uint16_t w = 0;

	if (cli->help) {
		cli_printf(cli, "%s [<exit status>]\n", cli->av[0]);
		cli_printf(cli, "\t\tExit emulator.\n");
		return;
	}
	cli->ac--;
	cli->av++;
	if (cli_n_m_args(cli, 0, 1, "[exit status]\n"))
		return;
	if (cli->ac == 1)
		w = atoi(cli->av[0]);
	cli_printf(cli, "Exit after %ju IOC instructions\n", ioc_nins);
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

void
cli_io_help(struct cli *cli, const char *desc, int has_trace, int has_elastic)
{
	cli_printf(cli, "%s\n", desc);
	cli_usage(cli, " ...\n");
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
cli_n_m_args(struct cli *cli, int minarg, int maxarg, const char *fmt, ...)
{
	va_list ap;

	assert (minarg <= maxarg);
	AN(fmt);
	if (cli->ac >= minarg && cli->ac <= maxarg)
		return (0);
	if (maxarg == 0) {
		(void)cli_error(cli, "Expected no arguments after '");
		cli_path(cli);
		cli_printf(cli,"'\n");
		return (1);
	}
	(void)cli_error(cli, "Wrong number of arguments after '");
	cli_path(cli);
	if (minarg != maxarg) {
		cli_printf(cli, "', expected: [%d…%d]", minarg, maxarg);
	} else {
		cli_printf(cli, "', expected: %d", minarg);
	}
	if (*fmt) {
		cli_printf(cli, ":\n\t");
		va_start(ap, fmt);
		(void)vprintf(fmt, ap);
		va_end(ap);
	} else {
		cli_printf(cli, "\n");
	}
	return (1);
}

int
cli_n_args(struct cli *cli, int args)
{
	return (cli_n_m_args(cli, args, args + 1, ""));
}

static cli_func_f cli_help;

static const struct cli_cmds cli_cmds[] = {
	{ "help",		cli_help },
	{ "exit",		cli_exit },
	{ "?",			cli_help },
	{ "console",		cli_ioc_console },
	{ "modem",		cli_ioc_modem },
	{ "ioc",		cli_ioc },
	{ "s",			cli_ioc_step },
	{ "reset",		cli_ioc_reset },
	{ "scsi_disk",		cli_scsi_disk },
	{ "scsi_tape",		cli_scsi_tape },
	{ "trace",		cli_trace },
	{ NULL,			NULL },
};

static void v_matchproto_(cli_func_t)
cli_help(struct cli *cli)
{

	cli->help = 1;
	cli->av0++;
	cli->av++;
	cli->ac--;
	cli_dispatch(cli, cli_cmds);
}

void
cli_path(struct cli *cli)
{
	size_t sz;
	char **av;
	const char *sep = "";

	for (av = cli->av0; *av; av++) {
		sz = strcspn(*av, " \t");
		if ((*av)[sz])
			cli_printf(cli, "%s\"%s\"", sep, *av);
		else
			cli_printf(cli, "%s%s", sep, *av);
		if (av == cli->av)
			break;
		sep = " ";
	}
}

void
cli_usage(struct cli *cli, const char *fmt, ...)
{
	va_list ap;

	cli_printf(cli, "Usage: ");
	if (*cli->av0)
		cli_path(cli);
	else
		cli_printf(cli, " ");

	va_start(ap, fmt);
	(void)vprintf(fmt, ap);
	va_end(ap);
}

void
cli_dispatch(struct cli *cli, const struct cli_cmds *cmds)
{
	const struct cli_cmds *cc;

	if (cli->ac == 0 && cli->help) {
		cli_usage(cli, " ...\n");
		for (cc = cmds; cc->cmd != NULL; cc++)
			cli_printf(cli, "\t%s\n", cc->cmd);
		return;
	}

	for (cc = cmds; cc->cmd != NULL; cc++)
		if (!strcasecmp(cc->cmd, *cli->av))
			break;

	if (cc->func != NULL) {
		cc->func(cli);
		return;
	}

	(void)cli_error(cli, "CLI error, no command: ");
	cli_path(cli);
	cli_printf(cli, "\n");
}

int
cli_exec(const char *s)
{
	int ac;
	char **av;
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
	memset(&cli, 0, sizeof cli);
	cli.ac = ac - 1;
	cli.av0 = av + 1;
	cli.av = av + 1;
	cli_dispatch(&cli, cli_cmds);
	VAV_Free(av);
	return (cli.status);
}

int
cli_from_file(FILE *fi, int fatal)
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
		rv = cli_exec(buf);
		if (rv < 0 || (rv && fatal))
			break;
	}
	return (rv);
}
