#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "F113.hh"

// (Dual) JK Negative Edge-Triggered Flip-Flop
// Fairchild DS009473 April 1988 Revised September 2000

struct scm_f113_state {
	struct ctx ctx;
	bool dreg;
};

void
SCM_F113 :: loadit(const char *arg)
{
	state = (struct scm_f113_state *)
	    CTX_Get("f113", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F113 :: doit(void)
{
	while (1) {
		const char *what;
		wait();
		state->ctx.activations++;
		if (IS_L(pin4)) {
			what = " PR ";
			state->dreg = true;
		} else if (!pin1.negedge()) {
			what = " ? ";
		} else if (IS_H(pin3) && IS_H(pin2)) {
			what = " inv ";
			state->dreg = !state->dreg;
		} else if (IS_L(pin3) && IS_H(pin2)) {
			what = " clr ";
			state->dreg = false;
		} else if (IS_H(pin3) && IS_L(pin2)) {
			what = " set ";
			state->dreg = true;
		} else {
			what = " nop ";
		}
		wait(5, SC_NS);
		TRACE(
		    << what
		    << " clk " << pin1
		    << " k " << pin2
		    << " j " << pin3
		    << " pr " << pin4
		    << " q "
		    << state->dreg
		);
		pin5 = AS(state->dreg);
		pin6 = AS(!state->dreg);
	}
}
