/*
 * PNLREG - 4 bit LED control register
 * -----------------------------------
 *
 * Chip Select: 0xfffff2xx
 *
 */

#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "Infra/r1000.h"
#include "Ioc/ioc.h"
#include "Ioc/memspace.h"

void v_matchproto_(mem_post_write)
plnreg_post_write(int debug, uint8_t *space, unsigned width, unsigned adr)
{

	ioc_bus_xact_schedule(0xfffff200 + adr, space[adr], width, 1, 0);

	if (space[adr])
		printf("PNLREG %x %x %x %x\n", debug, width, adr, space[adr]);
}

