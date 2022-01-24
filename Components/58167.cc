#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "58167.hh"

struct scm_58167_state {
	struct ctx ctx;
};

void
SCM_58167 :: loadit(const char *arg)
{
	state = (struct scm_58167_state *)CTX_Get("58167", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_58167 :: doit(void)
{
	state->ctx.activations++;
	TRACE(
	    <<pin1
	    <<pin2
	    <<pin3
	    <<pin4
	    <<pin5
	    <<pin6
	    <<pin7
	    <<pin8
	    <<pin9
	    <<pin10
	    <<pin14
	    <<pin15
	    <<pin16
	    <<pin17
	    <<pin18
	    <<pin19
	    <<pin20
	    <<pin21
	    <<pin22
	    <<pin23
	    <<pin24
	);
}
