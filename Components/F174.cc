#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "F174.hh"

// Hex D-Type Flip-Flop with Master Reset
// Fairchild DS009489 April 1988 Revised September 2000

struct scm_f174_state {
	struct ctx ctx;
	bool dreg[6];
};

void
SCM_F174 :: loadit(const char *arg)
{
	state = (struct scm_f174_state *)
	    CTX_Get("f174", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F174 :: doit(void)
{
	const char *what;
	while (1) {
		wait();
		state->ctx.activations++;
		if (IS_L(pin1)) {
			state->dreg[0] = false;
			state->dreg[1] = false;
			state->dreg[2] = false;
			state->dreg[3] = false;
			state->dreg[4] = false;
			state->dreg[5] = false;
			what = " CLR ";
		} else if (pin9.posedge()) {
			state->dreg[0] = IS_H(pin3);
			state->dreg[1] = IS_H(pin4);
			state->dreg[2] = IS_H(pin6);
			state->dreg[3] = IS_H(pin11);
			state->dreg[4] = IS_H(pin13);
			state->dreg[5] = IS_H(pin14);
			what = " CLK ";
			wait(5, SC_NS);
		} else {
			what = " ??? ";
		}
		if (memcmp(state->dreg, old_state, sizeof state->dreg)) {
			TRACE(
			    << what
			    << " clr_" << pin1
			    << " clk " << pin9
			    << " d " << pin3
			    << pin4
			    << pin6
			    << pin11
			    << pin13
			    << pin14
			    << "|"
			    << state->dreg[0]
			    << state->dreg[1]
			    << state->dreg[2]
			    << state->dreg[3]
			    << state->dreg[4]
			    << state->dreg[5]
			);
			memcpy(old_state, state->dreg, sizeof old_state);
		}
		pin2 = AS(state->dreg[0]);
		pin5 = AS(state->dreg[1]);
		pin7 = AS(state->dreg[2]);
		pin10 = AS(state->dreg[3]);
		pin12 = AS(state->dreg[4]);
		pin15 = AS(state->dreg[5]);
	}
}
