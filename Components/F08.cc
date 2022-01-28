#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F08.hh"

// (Quad) 2-input AND

struct scm_f08_state {
	struct ctx ctx;
};

void
SCM_F08 :: loadit(const char *arg)
{
	state = (struct scm_f08_state *)
	    CTX_Get("f08", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F08 :: doit(void)
{

	state->ctx.activations++;
	bool s = IS_H(pin1) && IS_H(pin2);
	TRACE(
	    << pin1
	    << pin2
	    << "|"
	    << s
	);
	pin3 = AS(s);
}
