#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "F37.hh"

// (Quad) Two-Input NAND Buffer

struct scm_f37_state {
	struct ctx ctx;
};

void
SCM_F37 :: loadit(const char *arg)
{
	state = (struct scm_f37_state *)
	    CTX_Get("f37", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F37 :: doit(void)
{
	while (1) {
		pin3 = AS(1);
		TRACE( << pin1 << pin2 << " 1" );
		if (IS_L(pin1)) {
			wait(pin1.posedge_event());
			state->ctx.activations++;
		}
		if (IS_L(pin2)) {
			wait(pin2.posedge_event());
			state->ctx.activations++;
		}
		if (IS_H(pin1) && IS_H(pin2)) {
			wait(5, SC_NS);
			TRACE( << pin1 << pin2 << " 0" );
			pin3 = AS(0);
		}
		if (IS_H(pin1) && IS_H(pin2)) {
			wait(pin1.negedge_event() | pin2.negedge_event());
			state->ctx.activations++;
			wait(5, SC_NS);
		}
	}
}
