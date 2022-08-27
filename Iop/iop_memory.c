#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Infra/r1000.h"
#include "Infra/vend.h"

#include "Iop/iop.h"
#include "Iop/memspace.h"
#include "Iop/iop_sc_68k20.hh"

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
	Trace(1, "Memory Failure: %s(%u) 0x%08x (= %x)",
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

	Memory_Init();
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

/**********************************************************************
 * SystemC interface for IOP RAM
 */

static int
is_covered(unsigned adr)
{
	if (adr >= 0x0400 && adr < 0x0800)
		return (1);
	if (adr >= 0xe610 && adr < 0xe810)
		return (1);
	if (adr >= 0x40000 && adr < 0x54010)
		return (1);
	return (0);
}


void
ram_pre_read(int debug, uint8_t *space, unsigned width, unsigned adr)
{
	uint32_t u, a2;

	(void)debug;
	if (is_covered(adr) || is_covered(adr + width - 1)) {
		a2 = adr & ~3;
		while (adr + width > a2) {
			u = ioc_bus_xact_schedule(5, a2, 0, 4, 0);
			Trace(trace_ioc_dma, "RPR 0x%08x <= 0x%08x", a2, u);
			vbe32enc(space + a2, u);
			a2 += 4;
		}
	}
}

void
ram_post_write(int debug, uint8_t *space, unsigned width, unsigned adr)
{
	uint32_t u, a2;

	(void)debug;
	if (is_covered(adr) || is_covered(adr + width - 1)) {
		a2 = adr & ~3;
		while (adr + width > a2) {
			u = vbe32dec(space + a2);
			Trace(trace_ioc_dma, "RPW 0x%08x => 0x%08x", a2, u);
			(void)ioc_bus_xact_schedule(5, a2, u, 4, 1);
			a2 += 4;
		}
	}
}

/**********************************************************************
 * I/O Address mapping
 *
 * The 0x600 comes from pin 6 & 10 pull-ups on L41 IOCp33
 */

void
dma_write(unsigned segment, unsigned address, void *src, unsigned len)
{
	unsigned int u, v, w;

	u = 0x600;
	u |= (segment & 0x7) << 6;
	u |= address >> 10;
	v = vbe32dec(io_map_space + u * 4L);
	v |= (address & 0x3ff);
	Trace(trace_ioc_dma, "DMA [0x%x] => 0x%08x", len, v);
	memcpy(ram_space + v, src, len);
	if (is_covered(v) || is_covered(v + len - 1)) {
		Trace(trace_ioc_dma, "DMA=>SC [0x%x] => 0x%08x", len, v);
		for (u = 0; u < len; u += 4) {
			w = vbe32dec(ram_space + v + u);
			(void)ioc_bus_xact_schedule(5, v + u, w, 4, 1);
		}
	}
}

void
dma_read(unsigned segment, unsigned address, void *src, unsigned len)
{
	unsigned int u, v, w;

	u = 0x600;
	u |= address >> 10;
	u |= (segment & 0x7) << 6;
	v = vbe32dec(io_map_space + u * 4L);
	v |= (address & 0x3ff);
	Trace(trace_ioc_dma, "DMA [0x%x] <= 0x%08x", len, v);
	if (is_covered(v) || is_covered(v + len - 1)) {
		Trace(trace_ioc_dma, "DMA<=SC [0x%x] => 0x%08x", len, v);
		for (u = 0; u < len; u += 4) {
			w = ioc_bus_xact_schedule(5, v + u, 0, 4, 0);
			vbe32enc(ram_space + v + u, w );
		}
	}
	memcpy(src, ram_space + v, len);
}

