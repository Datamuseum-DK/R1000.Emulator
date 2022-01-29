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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Infra/r1000.h"
#include "Infra/elastic.h"

struct elastic_match {
	struct elastic			*ep;
	char				*pattern;
	int				idx;
	int				match;
	struct elastic_subscriber	*ws;
	pthread_mutex_t			mtx;
	pthread_cond_t			cond;
};

static void
elastic_match_txfunc(void *priv, const void *src, size_t len)
{
	struct elastic_match *em = priv;
	const char *s = src;

	AZ(pthread_mutex_lock(&em->mtx));
	while (em->match || em->pattern == NULL)
		AZ(pthread_cond_wait(&em->cond, &em->mtx));
	for (; len > 0; s++, len--) {
		if (em->pattern[em->idx] != *s) {
			em->idx = 0;
			continue;
		}
		if (em->pattern[++em->idx] != '\0')
			continue;
		em->match = 1;
		AZ(pthread_cond_signal(&em->cond));
		printf("✓\n");
	}
	AZ(pthread_mutex_unlock(&em->mtx));
}

static void
elastic_match_new(struct elastic *ep)
{
	struct elastic_match *em;

	em = calloc(sizeof *em, 1);
	AN(em);
	em->ep = ep;
	AZ(pthread_mutex_init(&em->mtx, NULL));
	AZ(pthread_cond_init(&em->cond, NULL));
	em->pattern = NULL;
	em->idx = 0;
	em->ws = elastic_subscribe(ep, elastic_match_txfunc, em);
	ep->em = em;
}

static void
elastic_match_arm(const struct elastic *ep, const char *match)
{
	struct elastic_match *em;

	em = ep->em;
	AN(em);

	AZ(pthread_mutex_lock(&em->mtx));
	if (em->pattern != NULL)
		free(em->pattern);
	em->pattern = strdup(match);
	AN(em->pattern);
	em->idx = 0;
	em->match = 0;
	AZ(pthread_cond_signal(&em->cond));
	AZ(pthread_mutex_unlock(&em->mtx));
}

static void
elastic_match_wait(const struct elastic *ep)
{
	struct elastic_match *em;

	em = ep->em;
	AN(em);
	AZ(pthread_mutex_lock(&em->mtx));
	while (!em->match)
		AZ(pthread_cond_wait(&em->cond, &em->mtx));
	AZ(pthread_mutex_unlock(&em->mtx));

	/*
	 * This sleep delays long enough that the DOMUS TTY driver will
	 * be ready for a "tty << something" command.
	 */
	usleep(150000);
}

int v_matchproto_(cli_elastic_f)
cli_elastic_match(struct elastic *ep, struct cli *cli)
{
	char buf[2];

	AN(cli);
	if (cli->help) {
		cli_printf(cli, "\tmatch arm <string>\n");
		cli_printf(cli, "\t\tStart looking for string\n");
		cli_printf(cli, "\tmatch wait\n");
		cli_printf(cli, "\t\tWait for 'arm' to match\n");
		cli_printf(cli, "\tmatch expect <string>\n");
		cli_printf(cli, "\t\tShortcut for 'arm' + 'wait'\n");
		cli_printf(cli, "\tmatch xon\n");
		cli_printf(cli, "\t\tWait for XON character\n");
		return (0);
	}
	AN(ep);

	if (strcasecmp(*cli->av, "match"))
		return (0);
	cli->av++;
	cli->ac--;
	if (ep->em == NULL)
		elastic_match_new(ep);

	if (!strcasecmp(*cli->av, "arm")) {
		if (cli_n_args(cli, 1))
			return(1);
		elastic_match_arm(ep, cli->av[1]);
		cli->av += 2;
		cli->ac -= 2;
		return (1);
	}
	if (!strcasecmp(*cli->av, "wait")) {
		if (cli_n_args(cli, 0))
			return(1);
		cli->av += 1;
		cli->ac -= 1;
		elastic_match_wait(ep);
		return (1);
	}
	if (!strcasecmp(*cli->av, "expect")) {
		if (cli_n_args(cli, 1))
			return(1);
		elastic_match_arm(ep, cli->av[1]);
		elastic_match_wait(ep);
		cli->av += 2;
		cli->ac -= 2;
		return (1);
	}
	if (!strcasecmp(*cli->av, "xon")) {
		if (cli_n_args(cli, 0))
			return(1);
		elastic_match_arm(ep, "\x11");
		elastic_match_wait(ep);
		cli->av += 1;
		cli->ac -= 1;
		return (1);
	}
	if (!strcasecmp(*cli->av, "byte")) {
		if (cli_n_args(cli, 1))
			return(1);
		buf[0] = strtoul(cli->av[1], NULL, 0);
		buf[1] = 0x00;
		elastic_match_arm(ep, buf);
		elastic_match_wait(ep);
		cli->av += 2;
		cli->ac -= 2;
		return (1);
	}
	return (0);
}
