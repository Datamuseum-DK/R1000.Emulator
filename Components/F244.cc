#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F244.hh"

// Octal buffers (3-state)
// Philips IC15 1994 Dec 05

struct scm_f244_state {
	struct ctx ctx;
	int job1, job2;
	bool out[8];
};

SCM_F244 :: SCM_F244(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit1);
	sensitive << pin1 << pin2 << pin4 << pin6 << pin8;
	SC_METHOD(doit2);
	sensitive << pin19 << pin11 << pin13 << pin15 << pin17;

	state = (struct scm_f244_state *)
	    CTX_Get("f244", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	state->job1 = -1;
	state->job2 = -1;
}

#define XBUF_CLASS SCM_F244
#define XBUF_SENSE IS_H

#define XBUF_DOIT doit1
#define XBUF_OE pin1
#define XBUF_JOB job1

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

#define XBUF_DOIT doit2
#define XBUF_OE pin19
#define XBUF_JOB job2

#define XBUF_PINS() \
	PIN(4, pin17, pin3) \
	PIN(5, pin15, pin5) \
	PIN(6, pin13, pin7) \
	PIN(7, pin11, pin9)

#include "Components/xbuf_doit.h"
