#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "F04.hh"

// (Hex) Inverter

struct scm_f04_state {
	struct ctx ctx;
};

void
SCM_F04 :: loadit(const char *arg)
{
	state = (struct scm_f04_state *)
	    CTX_Get("f04", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F04 :: doit(void)
{
	while (1) {
		bool s = IS_L(pin1);
		wait(5, SC_NS);
		TRACE(
		    << pin1
		    << "|"
		    << s
		);
		pin2 = AS(s);
		if (IS_L(pin1) == s)
			wait();
		state->ctx.activations++;
	}
}
