#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "1489.hh"

struct scm_1489_state {
	struct ctx ctx;
};

void
SCM_1489 :: loadit(const char *arg)
{
	state = (struct scm_1489_state *)CTX_Get("1489", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_1489 :: doit(void)
{
	state->ctx.activations++;
	TRACE(
	    <<pin1
	    <<pin2
	    <<pin4
	    <<pin5
	    <<pin9
	    <<pin10
	    <<pin12
	    <<pin13
	);
}
