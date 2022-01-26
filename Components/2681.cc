#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "2681.hh"

struct scm_2681_state {
	struct ctx ctx;
};

void
SCM_2681 :: loadit(const char *arg)
{
	state = (struct scm_2681_state *)CTX_Get("2681", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_2681 :: doit(void)
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
	    <<pin16
	    <<pin17
	    <<pin18
	    <<pin19
	    <<pin22
	    <<pin23
	    <<pin24
	    <<pin25
	    <<pin31
	    <<pin32
	    <<pin33
	    <<pin34
	    <<pin35
	    <<pin36
	    <<pin37
	    <<pin38
	    <<pin39
	);
}
