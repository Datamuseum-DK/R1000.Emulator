#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F40.hh"

// (Dual) 4-input NAND buffer
// Philips 853-0053 96314 April 11, 1989

struct scm_f40_state {
	struct ctx ctx;
};

void
SCM_F40 :: loadit(const char *arg)
{
	state = (struct scm_f40_state *)
	    CTX_Get("f40", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F40 :: doit(void)
{
	state->ctx.activations++;
	bool s = !(
	    IS_H(pin1) && IS_H(pin2) && IS_H(pin4) && IS_H(pin5)
	);
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
