#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "r1000.h"

#include "memspace.h"

struct memevent {
	VTAILQ_ENTRY(memevent)	list;
	unsigned		lo;
	unsigned		hi;
	mem_event_f		*func;
	void			*priv;
};

const char * const mem_op_read = "read";
const char * const mem_op_debug_read = "dbg_read";
const char * const mem_op_write = "write";
const char * const mem_op_debug_write = "dbg_write";

const char *mem_error_cause;

static pthread_mutex_t mem_mtx = PTHREAD_MUTEX_INITIALIZER;

void
mem_peg_reset(unsigned lo, unsigned hi, unsigned pegval)
{
	unsigned u;
	uint8_t *peg;

	lo &= ~1;
	for(u = lo; u < hi; u += 2) {
		peg = mem_find_peg(u);
		AN(peg);
		*peg &= ~pegval;
	}
}

void
mem_peg_set(unsigned lo, unsigned hi, unsigned pegval)
{
	unsigned u;
	uint8_t *peg;

	lo &= ~1;
	for(u = lo; u < hi; u += 2) {
		peg = mem_find_peg(u);
		if (peg == NULL) {
			printf("No PEG at 0x%x\n", u);
			continue;
		}
		AN(peg);
		*peg |= pegval;
	}
}

void
mem_fail(const char *what, unsigned width,
    unsigned address, unsigned data)
{
	trace(TRACE_ALL, "Memory Failure: %s(%u) 0x%08x (= %x)\n",
	    what, width, address, data);
	printf("Memory Failure: %s(%u) 0x%08x (= %x)\n",
	    what, width, address, data);
}

void
mem_peg_check(const char *what, struct memdesc *md,
    unsigned adr, unsigned val, unsigned width, unsigned peg)
{
	struct memevent *mev, *mev2;
	int did = 0, i;

	AZ(pthread_mutex_lock(&mem_mtx));
	VTAILQ_FOREACH_SAFE(mev, &md->events, list, mev2) {
		if (adr < mev->lo || mev->hi <= adr)
			continue;
		AZ(pthread_mutex_unlock(&mem_mtx));
		i = mev->func(mev->priv, md, what, adr, val, width, peg);
		AZ(pthread_mutex_lock(&mem_mtx));
		if (i) {
			VTAILQ_REMOVE(&md->events, mev, list);
			free(mev);
		}
		did = 1;
	}
	if (!did)
		mem_peg_reset(adr, adr, PEG_CHECK);
	AZ(pthread_mutex_unlock(&mem_mtx));
}

void
mem_init(void)
{
	struct memdesc *md;
	unsigned u;

	for(u = 0; u < n_memdesc; u++) {
		md = memdesc[u];
		VTAILQ_INIT(&md->events);
	}
}

void
mem_peg_register(unsigned lo, unsigned hi, mem_event_f *func, void *priv)
{
	struct memdesc *md;
	struct memevent *mev;
	unsigned u;

	AZ(pthread_mutex_lock(&mem_mtx));
	for(u = 0; u < n_memdesc; u++) {
		md = memdesc[u];
		if (md->hi <= lo)
			continue;
		if (md->lo >= hi)
			continue;
		mev = calloc(sizeof *mev, 1);
		AN(mev);
		mev->lo = lo;
		mev->hi = hi;
		mev->func = func;
		mev->priv = priv;
		VTAILQ_INSERT_TAIL(&md->events, mev, list);
	}
	mem_peg_set(lo, hi, PEG_CHECK);
	AZ(pthread_mutex_unlock(&mem_mtx));
}

void
mem_peg_expunge(const void *priv)
{
	struct memdesc *md;
	struct memevent *mev, *mev2;
	unsigned u;

	AN(priv);
	AZ(pthread_mutex_lock(&mem_mtx));
	for(u = 0; u < n_memdesc; u++) {
		md = memdesc[u];
		VTAILQ_FOREACH_SAFE(mev, &md->events, list, mev2) {
			if (mev->priv == priv) {
				VTAILQ_REMOVE(&md->events, mev, list);
				free(mev);
			}
		}
	}
	AZ(pthread_mutex_unlock(&mem_mtx));
}
