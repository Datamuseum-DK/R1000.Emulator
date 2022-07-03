/*
 * XECOM XE1201 "MOSART™" internal modem
 * -------------------------------------
 *
 * Chip Select: 0xffffbxxx
 *
 */

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "Infra/r1000.h"
#include "Ioc/ioc.h"
#include "Ioc/memspace.h"

static int enough;

void v_matchproto_(mem_pre_read)
io_mosart_pre_read(int debug, uint8_t *space, unsigned width, unsigned adr)
{
	if (debug) return;
	if (adr == 3) {
		space[adr] = 0x45;
		irq_lower(&IRQ_INT_MODEM2);
	}
	Trace(trace_ioc_io, "MOSART R [%x] -> %x (w%d)", adr, space[adr], width);
	if (++enough == 20)
		exit(4);
}

void v_matchproto_(mem_post_write)
io_mosart_post_write(int debug, uint8_t *space, unsigned width, unsigned adr)
{
                
	if (debug) return;
        Trace(trace_ioc_io, "MOSART W [%x] <- %x (w%d)", adr, space[adr], width);
        if (adr == 5 && space[adr] == 0x40) {
		fc00_space[0] = 0xff;
		irq_raise(&IRQ_INT_MODEM2);
	}
}

