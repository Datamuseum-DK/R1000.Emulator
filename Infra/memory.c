#include <stdio.h>

#include "r1000.h"

#include "memspace.h"

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
mem_peg_check(const char *what, struct memdesc *md,
    unsigned a, unsigned d, unsigned peg)
{
	printf("PEGCHECK %s %p %s 0x%x 0x%x {%x}\n", what, md, md->name, a, d, peg);
	(void)what;
	(void)md;
	(void)a;
	(void)d;
}

void
mem_init(void)
{
	struct memdesc *md;
	unsigned u;

	for(u = 0; u < n_memdesc; u++) {
		md = memdesc[u];
		VTAILQ_INIT(&md->memevents);
	}
}
