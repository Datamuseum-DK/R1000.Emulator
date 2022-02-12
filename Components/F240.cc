#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F240.hh"

// Octal inverting buffers (3-state)
// Philips 2004 Feb 25

struct scm_f240_state {
	struct ctx ctx;
	int job1, job2;
	bool out[8];
};

SCM_F240 :: SCM_F240(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit1);
	sensitive << pin1 << pin2 << pin4 << pin6 << pin8;
	SC_METHOD(doit2);
	sensitive << pin19 << pin11 << pin13 << pin15 << pin17;

	state = (struct scm_f240_state *)
	    CTX_Get("f240", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	state->job1 = -1;
	state->job2 = -1;
}

#define XBUF_CLASS SCM_F240
#define XBUF_SENSE IS_L

#define XBUF_DOIT doit1
#define XBUF_OE pin1
#define XBUF_JOB job1
#define XBUF_TRACE << " top "

#define XBUF_PINS() \
	PIN(0, pin2, pin18) \
	PIN(1, pin4, pin16) \
	PIN(2, pin6, pin14) \
	PIN(3, pin8, pin12)

#include "Components/xbuf_doit.h"

#undef XBUF_DOIT
#undef XBUF_OE
#undef XBUF_JOB
#undef XBUF_PINS
#undef XBUF_TRACE

#define XBUF_DOIT doit2
#define XBUF_OE pin19
#define XBUF_JOB job2
#define XBUF_TRACE << " bot "

#define XBUF_PINS() \
	PIN(4, pin11, pin9) \
	PIN(5, pin13, pin7) \
	PIN(6, pin15, pin5) \
	PIN(7, pin17, pin3)

#include "Components/xbuf_doit.h"
