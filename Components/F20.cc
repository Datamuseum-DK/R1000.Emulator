#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F20.hh"

// (Dual) 4-input NAND gate
// Philips IC15 1989 Mar 03

struct scm_f20_state {
	struct ctx ctx;
};

void
SCM_F20 :: loadit(const char *arg)
{
	state = (struct scm_f20_state *)
	    CTX_Get("f20", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F20 :: doit(void)
{

	state->ctx.activations++;
	bool s =
	    !(IS_H(pin1) && IS_H(pin2) && IS_H(pin4) && IS_H(pin5));
	TRACE(
	    << pin1
	    << pin2
	    << pin4
	    << pin5
	    << "|"
	    << s
	);
	pin6 = AS(s);
}
