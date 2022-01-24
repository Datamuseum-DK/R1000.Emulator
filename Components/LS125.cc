#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "LS125.hh"

struct scm_LS125_state {
	struct ctx ctx;
};

void
SCM_LS125 :: loadit(const char *arg)
{
	state = (struct scm_LS125_state *)CTX_Get("LS125", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_LS125 :: doit(void)
{
	state->ctx.activations++;
	TRACE(
	    <<pin1
	    <<pin2
	    <<pin3
	);
	if (IS_L(pin1))
		pin3 = AS(IS_H(pin2));
	else
		pin3 = sc_logic_Z;
}
