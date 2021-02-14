#include <stdio.h>

#include "r1000.h"

#include "memspace.h"

const char *mem_error_cause;

void
mem_fail(const char *what, unsigned width, unsigned address, unsigned data)
{
	trace(TRACE_ALL, "Memory Failure: %s(%u) 0x%08x (= %x)\n",
	    what, width, address, data);
	printf("Memory Failure: %s(%u) 0x%08x (= %x)\n",
	    what, width, address, data);
}

void
mem_peg_check(const char *what, struct memdesc *md, unsigned a, unsigned d)
{
	(void)what;
	(void)md;
	(void)a;
	(void)d;
}
