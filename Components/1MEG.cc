#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "1MEG.hh"

struct scm_1MEG_state {
	struct ctx ctx;
};

void
SCM_1MEG :: loadit(const char *arg)
{
	state = (struct scm_1MEG_state *)CTX_Get("1MEG", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_1MEG :: doit(void)
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
	    <<pin10
	    <<pin11
	    <<pin12
	    <<pin13
	    <<pin14
	    <<pin15
	    <<pin16
	    <<pin17
	);
}
