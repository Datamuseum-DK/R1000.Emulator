#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "Infra/r1000.h"
#include "Infra/vqueue.h"

#include "Iop/iop.h"
#include "Iop/iop_sc_68k20.hh"

#include "Chassis/r1000sc_priv.h"

struct bus_xact {
	struct ioc_sc_bus_xact	xact[1];
	int			is_sync;
	int			is_done;
	VTAILQ_ENTRY(bus_xact)	list;
};

static VTAILQ_HEAD(, bus_xact) bus_xact_head =
    VTAILQ_HEAD_INITIALIZER(bus_xact_head);

static pthread_mutex_t bus_xact_mtx;
static pthread_cond_t bus_xact_cond;

void
ioc_bus_xact_init(void)
{
	AZ(pthread_mutex_init(&bus_xact_mtx, NULL));
	AZ(pthread_cond_init(&bus_xact_cond, NULL));
}

uint32_t
ioc_bus_xact_schedule(uint32_t adr, uint32_t data, int width,
    int is_write, int is_sync)
{
	struct bus_xact *bxp;

	Trace(trace_ioc_sc, "IOC_SC %08x %08x %x %d %d", adr, data, width, is_write, is_sync);
	if (!(sc_boards & R1K_BOARD_IOC) || !sc_started)
		return(data);

	bxp = calloc(sizeof *bxp, 1);
	AN(bxp);
	bxp->xact->address = adr;
	bxp->xact->data = data;
	bxp->xact->width = width;
	bxp->xact->is_write = is_write;
	if (!is_write)
		is_sync = 1;
	bxp->is_sync = is_sync;
	AZ(pthread_mutex_lock(&bus_xact_mtx));
	VTAILQ_INSERT_TAIL(&bus_xact_head, bxp, list);
	while (is_sync && !bxp->is_done)
		AZ(pthread_cond_wait(&bus_xact_cond, &bus_xact_mtx));
	AZ(pthread_mutex_unlock(&bus_xact_mtx));
	if (is_sync) {
		data = bxp->xact->data;
		free(bxp);
	}
	return (data);
}

struct ioc_sc_bus_xact *
ioc_sc_bus_get_xact(void)
{
	struct bus_xact *bxp;

	if (VTAILQ_EMPTY(&bus_xact_head))
		return (NULL);

	AZ(pthread_mutex_lock(&bus_xact_mtx));
	bxp = VTAILQ_FIRST(&bus_xact_head);
	AZ(pthread_mutex_unlock(&bus_xact_mtx));
	if (bxp)
		return (bxp->xact);
	return (NULL);
}

void
ioc_sc_bus_done(struct ioc_sc_bus_xact **bxpa)
{
	struct bus_xact *bxp;

	AN(bxpa);
	AZ(pthread_mutex_lock(&bus_xact_mtx));
	bxp = VTAILQ_FIRST(&bus_xact_head);
	assert(bxp->xact == *bxpa);
	*bxpa = NULL;
	VTAILQ_REMOVE(&bus_xact_head, bxp, list);
	bxp->is_done = 1;
	if (bxp->is_sync)
		AZ(pthread_cond_signal(&bus_xact_cond));
	AZ(pthread_mutex_unlock(&bus_xact_mtx));
	if (!bxp->is_sync)
		free(bxp);
}

