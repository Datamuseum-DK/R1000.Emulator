#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "F02.hh"

// (Quad) 2-input NOR

struct scm_f02_state {
	struct ctx ctx;
};

void
SCM_F02 :: loadit(const char *arg)
{
	state = (struct scm_f02_state *)
	    CTX_Get("f02", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F02 :: doit(void)
{
	state->ctx.activations++;
	bool s = !(IS_H(pin1) || IS_H(pin2));
	TRACE(
	    <<pin1
	    <<pin2
	    << "|"
	    << s
	);
	pin3 = AS(s);
}
