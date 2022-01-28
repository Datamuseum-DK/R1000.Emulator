#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F521.hh"

// 8-Bit Identity Comparator
// Fairchild DS009545 April 1988 Revised October 2000

struct scm_f521_state {
	struct ctx ctx;
};

void
SCM_F521 :: loadit(const char *arg)
{
	state = (struct scm_f521_state *)
	    CTX_Get("f521", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F521 :: doit(void)
{
	state->ctx.activations++;
	bool s = IS_H(pin1) ||
	    IS_H(pin2) != IS_H(pin3) ||
	    IS_H(pin4) != IS_H(pin5) ||
	    IS_H(pin6) != IS_H(pin7) ||
	    IS_H(pin8) != IS_H(pin9) ||
	    IS_H(pin11) != IS_H(pin12) ||
	    IS_H(pin13) != IS_H(pin14) ||
	    IS_H(pin15) != IS_H(pin16) ||
	    IS_H(pin17) != IS_H(pin18);
	TRACE(
	    << " Ia=b " << pin1
	    << " A " << pin2 << pin4 << pin6 << pin8
	    << pin11 << pin13 << pin15 << pin17

	    << " B " << pin3 << pin5 << pin7 << pin9
	    << pin12 << pin14 << pin16 << pin18
	    << " = "
	    << s
	);
	pin19 = AS(s);
}
