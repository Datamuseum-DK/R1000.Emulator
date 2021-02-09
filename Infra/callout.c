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

#include <stdlib.h>
#include <pthread.h>
#include "r1000.h"

struct callout {
	VTAILQ_ENTRY(callout)		next;
	struct sim			*cs;
	nanosec				when;
	nanosec				repeat;
	const struct callout_how	*how;
	void				*priv;
};

typedef void callout_func_f(const struct callout *);

struct callout_how {
	const char			*name;
	callout_func_f			*func;
};

static void
callout_insert(struct callout *co)
{
	struct callout *co2;

	AN(co);
	AN(co->cs);
	AN(co->how);

	AZ(pthread_mutex_lock(&co->cs->callout_mtx));
	VTAILQ_FOREACH(co2, &co->cs->callouts, next)
		if (co2->when > co->when)
			break;
	if (co2 == NULL)
		VTAILQ_INSERT_TAIL(&co->cs->callouts, co, next);
	else
		VTAILQ_INSERT_BEFORE(co2, co, next);
	AZ(pthread_mutex_unlock(&co->cs->callout_mtx));
}

/**********************************************************************/

struct priv_sig_cond {
	pthread_mutex_t		*mtx;
	pthread_cond_t		*cond;
};

static void
callout_func_signal_cond(const struct callout *co)
{
	struct priv_sig_cond *psc;

	psc = co->priv;
	AZ(pthread_mutex_lock(psc->mtx));
	AZ(pthread_cond_signal(psc->cond));
	AZ(pthread_mutex_unlock(psc->mtx));
}

static const struct callout_how callout_signal_cond_how = {
	.name = "Signal Condvar",
	.func = callout_func_signal_cond,
};

void
callout_signal_cond(struct sim *cs, pthread_cond_t *cond,
    pthread_mutex_t *mtx, nanosec when, nanosec repeat)
{
	struct callout *co;
	struct priv_sig_cond *psc;
	char *p;

	p = calloc(sizeof *co + sizeof *psc, 1);
	AN(p);
	co = (void*)p;
	psc = (void*)(p + sizeof *co);
	psc->cond = cond;
	psc->mtx = mtx;
	co->cs = cs;
	co->when = cs->simclock + when;
	co->repeat = repeat;
	co->priv = psc;
	co->how = &callout_signal_cond_how;
	callout_insert(co);
}

/**********************************************************************/

struct priv_sig_callback {
	callout_callback_f	*func;
	void			*priv;
};

static void
callout_func_callback(const struct callout *co)
{
	struct priv_sig_callback *psc;

	psc = co->priv;
	psc->func(psc->priv);
}

static const struct callout_how callout_callback_how = {
	.name = "Callback",
	.func = callout_func_callback,
};

void
callout_callback(struct sim *cs, callout_callback_f *func,
    void *priv, nanosec when, nanosec repeat)
{
	struct callout *co;
	struct priv_sig_callback *psc;
	char *p;

	p = calloc(sizeof *co + sizeof *psc, 1);
	AN(p);
	co = (void*)p;
	psc = (void*)(p + sizeof *co);
	psc->func = func;
	psc->priv = priv;
	co->cs = cs;
	co->when = cs->simclock + when;
	co->repeat = repeat;
	co->priv = psc;
	co->how = &callout_callback_how;
	callout_insert(co);
}

/**********************************************************************/

nanosec
callout_poll(struct sim *cs)
{
	struct callout *co;
	nanosec rv = 0;

	while (1) {
		AZ(pthread_mutex_lock(&cs->callout_mtx));
		co = VTAILQ_FIRST(&cs->callouts);
		if (co != NULL && co->when < cs->simclock) {
			VTAILQ_REMOVE(&cs->callouts, co, next);
		} else {
			if (co != NULL)
				rv = co->when;
			co = NULL;
		}
		AZ(pthread_mutex_unlock(&cs->callout_mtx));
		if (co == NULL)
			return (rv);
		co->how->func(co);
		if (co->repeat > 0) {
			co->when += co->repeat;
			callout_insert(co);
		} else {
			free(co);
		}
	}
}
