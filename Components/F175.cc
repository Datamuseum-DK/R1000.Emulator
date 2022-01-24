#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "F175.hh"

// Quad D-Type Flip-Flop
// Fairchild DS009490 April 1988 Revised September 2000

struct scm_f175_state {
	struct ctx ctx;
	bool dreg[4];
};

void
SCM_F175 :: loadit(const char *arg)
{
	state = (struct scm_f175_state *)
	    CTX_Get("f175", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F175 :: doit(void)
{
	while (1) {
		wait();
		state->ctx.activations++;
		if (IS_L(pin1)) {
			state->dreg[0] = false;
			state->dreg[1] = false;
			state->dreg[2] = false;
			state->dreg[3] = false;
		} else if (pin9.posedge()) {
			state->dreg[0] = IS_H(pin4);
			state->dreg[1] = IS_H(pin5);
			state->dreg[2] = IS_H(pin12);
			state->dreg[3] = IS_H(pin13);
			wait(5, SC_NS);
		}
		if (memcmp(state->dreg, old_state, sizeof state->dreg)) {
			TRACE(
			    << " clr_ " << pin1
			    << " clk " << pin9
			    << " d " << pin4
			    << pin5
			    << pin12
			    << pin13
			    << " |"
			    << " q " << state->dreg[0]
			    << state->dreg[1]
			    << state->dreg[2]
			    << state->dreg[3]
			);
			memcpy(old_state, state->dreg, sizeof old_state);
		}
		pin2 = AS(state->dreg[0]);
		pin3 = AS(!state->dreg[0]);
		pin7 = AS(state->dreg[1]);
		pin6 = AS(!state->dreg[1]);
		pin10 = AS(state->dreg[2]);
		pin11 = AS(!state->dreg[2]);
		pin15 = AS(state->dreg[3]);
		pin14 = AS(!state->dreg[3]);
	}
}
