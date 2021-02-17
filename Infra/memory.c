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
mem_peg_check(const char *what, const struct memdesc *md,
    unsigned adr, unsigned val, unsigned width, unsigned peg)
{
	struct memevent *mev;

	VTAILQ_FOREACH(mev, &md->events, list) {
		if (mev->lo <= adr && adr < mev->hi)
			mev->func(mev->priv, what, adr, val, width, peg);
	}
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
mem_peg_set(unsigned lo, unsigned hi, unsigned pegval)
{
	unsigned u;
	uint8_t *peg;

	lo &= ~1;
	for(u = lo; u < hi; u += 2) {
		peg = mem_find_peg(u);
		AN(peg);
		*peg |= pegval;
	}
}

void
mem_peg_register(unsigned lo, unsigned hi, mem_event_f *func, void *priv)
{
	struct memdesc *md;
	struct memevent *mev;
	unsigned u;

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
}
