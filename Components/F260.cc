#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F260.hh"

// (Dual) 5-input NOR gate
// Philips IC15 1988 Nov 29

struct scm_f260_state {
	struct ctx ctx;
};

void
SCM_F260 :: loadit(const char *arg)
{
	state = (struct scm_f260_state *)
	    CTX_Get("f260", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F260 :: doit(void)
{

	state->ctx.activations++;
	bool s = !(IS_H(pin1) || IS_H(pin2) || IS_H(pin3)
	    || IS_H(pin12) || IS_H(pin13));
	TRACE(
	    << pin1
	    << pin2
	    << pin3
	    << pin12
	    << pin13
	    << "|"
	    << s
	);
	pin5 = AS(s);
}
