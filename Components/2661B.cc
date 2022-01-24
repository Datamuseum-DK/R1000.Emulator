#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "2661B.hh"

struct scm_2661B_state {
	struct ctx ctx;
};

void
SCM_2661B :: loadit(const char *arg)
{
	state = (struct scm_2661B_state *)CTX_Get("2661B", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_2661B :: doit(void)
{

	state->ctx.activations++;
	TRACE(
	    <<pin1
	    <<pin2
	    <<pin3
	    <<pin5
	    <<pin6
	    <<pin7
	    <<pin8
	    <<pin9
	    <<pin10
	    <<pin11
	    <<pin12
	    <<pin13
	    <<pin16
	    <<pin17
	    <<pin20
	    <<pin21
	    <<pin22
	    <<pin27
	    <<pin28
	);
}
