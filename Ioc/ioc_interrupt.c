#include <stdlib.h>
#include <pthread.h>

#include "r1000.h"
#include "ioc.h"
#include "vqueue.h"

struct irq_vector {
	const char			*name;
	unsigned			vector;
	unsigned			level;
	unsigned			priority;
	unsigned			pending;
	VTAILQ_ENTRY(irq_vector)	list;
};

static VTAILQ_HEAD(, irq_vector) pending = VTAILQ_HEAD_INITIALIZER(pending);

// We dont know yet...
#define L67	6

#define IRQ_VECTOR(upper, level, vector, priority) \
	struct irq_vector IRQ_##upper = { #upper, vector, level, priority };
INTERRUPT_TABLE
#undef IRQ_VECTOR

static pthread_mutex_t irq_mtx = PTHREAD_MUTEX_INITIALIZER;

unsigned irq_level = 0x0;

void
irq_raise(struct irq_vector *vp)
{
	struct irq_vector *vp2;

	AZ(pthread_mutex_lock(&irq_mtx));
	if (!vp->pending) {
		Trace(trace_ioc_interrupt, "IRQ +%s", vp->name);
		vp->pending = 1;
		VTAILQ_FOREACH(vp2, &pending, list) {
			if (vp2->priority > vp->priority) {
				VTAILQ_INSERT_BEFORE(vp2, vp, list);
				break;
			}
		}
		if (vp2 == NULL)
			VTAILQ_INSERT_TAIL(&pending, vp,  list);
		vp = VTAILQ_FIRST(&pending);
		irq_level = vp->level;
	} else {
		Trace(trace_ioc_interrupt, "IRQ (+%s)", vp->name);
	}
	AZ(pthread_mutex_unlock(&irq_mtx));
}

void
irq_lower(struct irq_vector *vp)
{
	AZ(pthread_mutex_lock(&irq_mtx));
	if (vp->pending) {
		Trace(trace_ioc_interrupt, "IRQ -%s", vp->name);
		vp->pending = 0;
		VTAILQ_REMOVE(&pending, vp, list);
		vp = VTAILQ_FIRST(&pending);
		if (vp != NULL) {
			irq_level = vp->level;
		} else {
			irq_level = 0;
		}
	} else {
		Trace(trace_ioc_interrupt, "IRQ (-%s)", vp->name);
	}
	AZ(pthread_mutex_unlock(&irq_mtx));
}

unsigned
irq_getvector(unsigned int arg)
{
	struct irq_vector *vp;
	unsigned retval;

	AZ(pthread_mutex_lock(&irq_mtx));
	vp = VTAILQ_FIRST(&pending);
	if (vp != NULL) {
		retval = vp->vector;
		Trace(trace_ioc_interrupt,
		    "VECTOR 0x%x %s (arg=0x%x)", retval,
		    vp->name, arg);
	} else {
		retval = 0;
		Trace(trace_ioc_interrupt,
		    "VECTOR 0x%x (arg=0x%x)", 0, arg);
	}
	AZ(pthread_mutex_unlock(&irq_mtx));
	return (retval);
}
