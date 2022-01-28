#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F74.hh"

// (Dual) D-Type Positive Edge-Triggered Flip-Flop
// Fairchild DS009469 April 1988 Revised September 2000

struct scm_f74_state {
	struct ctx ctx;
	bool dreg[2];
};

void
SCM_F74 :: loadit(const char *arg)
{
	state = (struct scm_f74_state *)
	    CTX_Get("f74", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F74 :: doit(void)
{
	bool oldreg[2];

	while (1) {
		const char *what = " ? ";
		wait();
		state->ctx.activations++;
		memcpy(oldreg, state->dreg, sizeof oldreg);
		if (IS_L(pin1) && IS_L(pin4)) {
			state->dreg[0] = state->dreg[1] = true;
			what = " 2L ";
		} else if (IS_L(pin1)) {
			state->dreg[0] = false;
			state->dreg[1] = true;
			what = " CLR ";
		} else if (IS_L(pin4)) {
			state->dreg[0] = true;
			state->dreg[1] = false;
			what = " SET ";
		} else if (pin3.posedge()) {
			state->dreg[0] = IS_H(pin2);
			state->dreg[1] = !state->dreg[0];
			what = " CLK ";
		}
		wait(5, SC_NS);
		if (memcmp(oldreg, state->dreg, sizeof oldreg)) {
			TRACE(
			    << what
			    << pin1
			    << pin2
			    << pin3
			    << pin4
			    << "|"
			    << state->dreg[0]
			    << state->dreg[1]
			);
		}
		pin5 = AS(state->dreg[0]);
		pin6 = AS(state->dreg[1]);
	}
}
