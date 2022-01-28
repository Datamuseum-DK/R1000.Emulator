#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F38.hh"

// (Quad) Two-Input NAND Buffer (Open Collector)
// Fairchild DS009465 April 1988 Revised September 2000

struct scm_f38_state {
	struct ctx ctx;
};

void
SCM_F38 :: loadit(const char *arg)
{
	state = (struct scm_f38_state *)
	    CTX_Get("f38", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F38 :: doit(void)
{
	state->ctx.activations++;

	bool s = !(IS_H(pin1) && IS_H(pin2));
	TRACE(
	    << pin1
	    << pin2
	    << "|"
	    << s
	);
	pin3 = AS(s);
}
