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

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "r1000.h"
#include "elastic.h"

struct elastic *
elastic_new(int mode)
{
	struct elastic *ep;

	ep = calloc(1, sizeof *ep);
	AN(ep);
	VTAILQ_INIT(&ep->chunks_in);
	VTAILQ_INIT(&ep->chunks_out);
	VTAILQ_INIT(&ep->subscribers);
	AZ(pthread_mutex_init(&ep->mtx, NULL));
	AZ(pthread_cond_init(&ep->cond_in, NULL));
	AZ(pthread_cond_init(&ep->cond_out, NULL));
	assert(mode == O_RDONLY || mode == O_RDWR || mode == O_WRONLY);
	ep->text = 1;
	ep->mode = mode;
	ep->bits_per_char = 8;
	return (ep);
}

/* Output Subscriptions ***********************************************/

static void *
elastic_subscriber_thread(void *priv)
{
	struct elastic_subscriber *esp = priv;
	struct elastic *ep = esp->ep;
	struct chunk *cp;

	AZ(pthread_mutex_lock(&ep->mtx));
	while (1) {
		while (!esp->die && VTAILQ_EMPTY(&esp->chunks))
			AZ(pthread_cond_wait(&esp->cond, &ep->mtx));
		cp = VTAILQ_FIRST(&esp->chunks);
		if (cp != NULL)
			VTAILQ_REMOVE(&esp->chunks, cp, next);
		AZ(pthread_mutex_unlock(&ep->mtx));
		if (esp->die && cp == NULL)
			break;
		AN(cp);
		AN(esp->func);
		AN(esp->priv);
		AN(cp->ptr);
		AN(cp->len);
		if (!esp->die)
			esp->func(esp->priv, cp->ptr, cp->len);
		free(cp->ptr);
		free(cp);
		AZ(pthread_mutex_lock(&ep->mtx));
	}
	AZ(pthread_cond_destroy(&esp->cond));
	free(esp);
	return (NULL);
}

struct elastic_subscriber *
elastic_subscribe(struct elastic *ep, elastic_deliver_f *func, void *priv)
{
	struct elastic_subscriber *esp;

	assert(ep->mode != O_RDONLY);
	esp = calloc(1, sizeof *esp);
	AN(esp);
	esp->ep = ep;
	esp->func = func;
	esp->priv = priv;
	AZ(pthread_cond_init(&esp->cond, NULL));
	VTAILQ_INIT(&esp->chunks);
	AZ(pthread_mutex_lock(&ep->mtx));
	VTAILQ_INSERT_TAIL(&ep->subscribers, esp, next);
	if (!VTAILQ_EMPTY(&ep->chunks_out))
		VTAILQ_CONCAT(&esp->chunks, &ep->chunks_out, next);
	AZ(pthread_mutex_unlock(&ep->mtx));
	AZ(pthread_create(&esp->thread, NULL, elastic_subscriber_thread, esp));
	AZ(pthread_detach(esp->thread));
	return (esp);
}

void
elastic_unsubscribe(struct elastic *ep, struct elastic_subscriber *esp)
{

	AN(ep);
	AN(esp);
	AZ(pthread_mutex_lock(&ep->mtx));
	esp->die = 1;
	VTAILQ_REMOVE(&ep->subscribers, esp, next);
	AZ(pthread_mutex_unlock(&ep->mtx));
	AZ(pthread_cond_signal(&esp->cond));
}

/* Input injections ***************************************************/

void
elastic_inject(struct elastic *ep, const void *ptr, ssize_t len)
{
	struct chunk *cp;

	if (len < 0)
		len = strlen(ptr);
	if (len == 0)
		return;
	cp = calloc(1, sizeof *cp);
	AN(cp);
	cp->ptr = malloc(len);
	AN(cp->ptr);
	memcpy(cp->ptr, ptr, len);
	cp->len = len;
	AZ(pthread_mutex_lock(&ep->mtx));
	VTAILQ_INSERT_TAIL(&ep->chunks_in, cp, next);
	AZ(pthread_cond_signal(&ep->cond_in));
	AZ(pthread_mutex_unlock(&ep->mtx));
}

/* Driver Interface ***************************************************/

nanosec
elastic_nsec_per_char(const struct elastic *ep)
{

	AN(ep);
	return(ep->bits_per_char * 1000000000 / ep->bits_per_sec);
}

static struct chunk *
mk_chunk(const void *ptr, ssize_t len)
{
	struct chunk *cp;

	cp = calloc(1, sizeof *cp);
	AN(cp);
	cp->ptr = malloc(len);
	AN(cp->ptr);
	memcpy(cp->ptr, ptr, len);
	cp->len = len;
	return (cp);
}

void
elastic_put(struct elastic *ep, const void *ptr, ssize_t len)
{
	struct chunk *cp;
	struct elastic_subscriber *esp;

	assert(ep->mode != O_RDONLY);
	if (len < 0)
		len = strlen(ptr);
	if (len == 0)
		return;
	AZ(pthread_mutex_lock(&ep->mtx));
	if (VTAILQ_EMPTY(&ep->subscribers)) {
		cp = mk_chunk(ptr, len);
		VTAILQ_INSERT_TAIL(&ep->chunks_out, cp, next);
	} else {
		VTAILQ_FOREACH(esp, &ep->subscribers, next) {
			cp = mk_chunk(ptr, len);
			VTAILQ_INSERT_TAIL(&esp->chunks, cp, next);
			AZ(pthread_cond_signal(&esp->cond));
		}
	}
	AZ(pthread_mutex_unlock(&ep->mtx));
}

ssize_t
elastic_get(struct elastic *ep, void *ptr, ssize_t len)
{
	struct chunk *cp;

	assert(ep->mode != O_WRONLY);
	AZ(pthread_mutex_lock(&ep->mtx));
	while (VTAILQ_EMPTY(&ep->chunks_in))
		AZ(pthread_cond_wait(&ep->cond_in, &ep->mtx));
	cp = VTAILQ_FIRST(&ep->chunks_in);
	if (cp->len - cp->read < len)
		len = cp->len - cp->read;
	memcpy(ptr, cp->ptr + cp->read, len);
	cp->read += len;
	if (cp->read == cp->len) {
		VTAILQ_REMOVE(&ep->chunks_in, cp, next);
		free(cp->ptr);
		free(cp);
	}
	AZ(pthread_mutex_unlock(&ep->mtx));
	return (len);
}

int
elastic_empty(const struct elastic *ep)
{
	return(VTAILQ_EMPTY(&ep->chunks_in));
}

int v_matchproto_(cli_elastic_f)
cli_elastic(struct elastic *ep, struct cli *cli)
{
	uint8_t u8;

	AN(cli);
	if (cli->help) {
		cli_printf(cli, "<elastic> [arguments]\n");
		cli_printf(cli, "\ttext\n");
		cli_printf(cli, "\t\tSet text mode\n");
		cli_printf(cli, "\tbinary\n");
		cli_printf(cli, "\t\tSet binary mode\n");
		cli_printf(cli, "\t\tElastic buffer arguments\n");
		cli_printf(cli, "\tcps <characters_per_second>\n");
		cli_printf(cli, "\t\tOutput bandwidth\n");
		cli_printf(cli, "\tbaud <baud_rate>\n");
		cli_printf(cli, "\t\tOutput bandwidth\n");
		cli_printf(cli, "\t<< <string>\n");
		cli_printf(cli, "\t\tInput <string> + CR into buffer\n");
		(void)cli_elastic_fd(NULL, cli);
		(void)cli_elastic_tcp(NULL, cli);
		(void)cli_elastic_match(NULL, cli);
		return (0);
	}

	AN(ep);

	if (!strcmp(*cli->av, "cps")) {
		if (cli->ac != 1) {
			if (cli_n_args(cli, 1))
				return (1);
			ep->bits_per_sec =
			    atoi(cli->av[1]) * ep->bits_per_char;
			cli->ac--;
			cli->av++;
		}
		cli_printf(cli,
		    "cps = %jd\n", ep->bits_per_sec / ep->bits_per_char);
		cli->ac--;
		cli->av++;
		return(1);
	}
	if (!strcmp(*cli->av, "baud")) {
		if (cli->ac != 1) {
			if (cli_n_args(cli, 1))
				return (1);
			ep->bits_per_sec = atoi(cli->av[1]);
			cli->ac--;
			cli->av++;
		}
		cli_printf(cli, "baud = %jd\n", ep->bits_per_sec);
		cli->ac--;
		cli->av++;
		return(1);
	}
	if (!strcmp(*cli->av, "text")) {
		if (cli_n_args(cli, 0))
			return (1);
		ep->text = 1;
		cli->ac--;
		cli->av++;
		return (1);
	}

	if (!strcmp(*cli->av, "binary")) {
		if (cli_n_args(cli, 0))
			return (1);
		ep->text = 0;
		cli->ac--;
		cli->av++;
		return (1);
	}

	if (!strcmp(*cli->av, "hex")) {
		if (cli_n_args(cli, 1))
			return (1);
		if (ep->mode == O_WRONLY)
			return (cli_error(cli, "Only inputs can '<<'\n"));
		u8 = (uint8_t)strtoul(cli->av[1], NULL, 16);
		elastic_inject(ep, &u8, 1);
		cli->av += 2;
		cli->ac -= 2;
		return (1);
	}
	if (!strcmp(*cli->av, "<<")) {
		if (cli_n_args(cli, 1))
			return (1);
		if (ep->mode == O_WRONLY)
			return (cli_error(cli, "Only inputs can '<<'\n"));
		elastic_inject(ep, cli->av[1], -1);
		elastic_inject(ep, "\r", -1);
		cli->av += 2;
		cli->ac -= 2;
		return (1);
	}
	if (cli_elastic_fd(ep, cli))
		return (1);
	if (cli_elastic_tcp(ep, cli))
		return (1);
	if (cli_elastic_match(ep, cli))
		return (1);
	return (0);
}
