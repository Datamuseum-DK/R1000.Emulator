#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "DLY_35.hh"

struct scm_DLY_35_state {
	struct ctx ctx;
};

void
SCM_DLY_35 :: loadit(const char *arg)
{
	state = (struct scm_DLY_35_state *)CTX_Get("DLY_35", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_DLY_35 :: doit(void)
{
	state->ctx.activations++;
	TRACE(
	    <<pin1
	);
}
