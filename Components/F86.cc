#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F86.hh"

// (Quad) 2-input
// Philips IC15 1990 Feb 09

struct scm_f86_state {
	struct ctx ctx;
};

void
SCM_F86 :: loadit(const char *arg)
{
	state = (struct scm_f86_state *)
	    CTX_Get("f86", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F86 :: doit(void)
{

	state->ctx.activations++;
	bool s = IS_H(pin1) ^ IS_H(pin2);
	TRACE(
	    << pin1
	    << pin2
	    << "|"
	    << s
	);
	pin3 = AS(s);
}
