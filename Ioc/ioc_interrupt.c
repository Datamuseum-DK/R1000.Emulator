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

#define DEFAULT_VECTOR	0x50		// See IOC_EEPROM 0x800011f0

static unsigned cur_vector = DEFAULT_VECTOR;
unsigned irq_level = 0x0;

void
irq_raise(struct irq_vector *vp)
{
	struct irq_vector *vp2;

	AZ(pthread_mutex_lock(&irq_mtx));
	if (!vp->pending) {
		trace(4, "IRQ +%s\n", vp->name);
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
		cur_vector = vp->vector;
		irq_level = vp->level;
	} else {
		trace(4, "IRQ (+%s)\n", vp->name);
	}
	AZ(pthread_mutex_unlock(&irq_mtx));
}

void
irq_lower(struct irq_vector *vp)
{
	AZ(pthread_mutex_lock(&irq_mtx));
	if (vp->pending) {
		trace(4, "IRQ -%s\n", vp->name);
		vp->pending = 0;
		VTAILQ_REMOVE(&pending, vp, list);
		vp = VTAILQ_FIRST(&pending);
		if (vp != NULL) {
			cur_vector = vp->vector;
			irq_level = vp->level;
		} else {
			cur_vector = DEFAULT_VECTOR;
			irq_level = 0;
		}
	} else {
		trace(4, "IRQ (-%s)\n", vp->name);
	}
	AZ(pthread_mutex_unlock(&irq_mtx));
}

unsigned
irq_getvector(unsigned int arg)
{
	unsigned retval;
	AZ(pthread_mutex_lock(&irq_mtx));
	trace(TRACE_68K, "VECTOR 0x%x (arg=0x%x)\n", cur_vector, arg);
	retval = cur_vector;
	AZ(pthread_mutex_unlock(&irq_mtx));
	return (retval);
}
