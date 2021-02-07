#include <stdlib.h>
#include <pthread.h>

#include "r1000.h"
#include "ioc.h"
#include "vqueue.h"

struct irq_vector {
	const char			*name;
	unsigned			vector;
	unsigned			level;
	unsigned			pending;
	VTAILQ_ENTRY(irq_vector)	list;
};

static VTAILQ_HEAD(, irq_vector) pending = VTAILQ_HEAD_INITIALIZER(pending);

// We dont know yet...
#define L67	6

#define IRQ_VECTOR(upper, level, vector) \
	struct irq_vector IRQ_##upper = { #upper, vector, level };
INTERRUPT_TABLE
#undef IRQ_VECTOR

static pthread_mutex_t irq_mtx = PTHREAD_MUTEX_INITIALIZER;

#define DEFAULT_VECTOR	0x50		// See IOC_EEPROM 0x800011f0

static unsigned cur_vector = DEFAULT_VECTOR;
unsigned irq_level = 0x0;

int
irq_raise(struct irq_vector *vp)
{
	trace(4, "IRQ +%s", vp->name);
	AZ(pthread_mutex_lock(&irq_mtx));
	AZ(vp->pending);
	vp->pending = 1;
	VTAILQ_INSERT_TAIL(&pending, vp,  list);
	vp = VTAILQ_FIRST(&pending);
	cur_vector = vp->vector;
	irq_level = vp->level;
	AZ(pthread_mutex_unlock(&irq_mtx));
	return (1);
}

int
irq_lower(struct irq_vector *vp)
{
	trace(4, "IRQ -%s", vp->name);
	AZ(pthread_mutex_lock(&irq_mtx));
	AN(vp->pending);
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
	AZ(pthread_mutex_unlock(&irq_mtx));
	return (0);
}

unsigned
irq_getvector(void)
{
	unsigned retval;
	AZ(pthread_mutex_lock(&irq_mtx));
	retval = cur_vector;
	AZ(pthread_mutex_unlock(&irq_mtx));
	return (retval);
}
