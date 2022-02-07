#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F113.hh"

// (Dual) JK Negative Edge-Triggered Flip-Flop
// Fairchild DS009473 April 1988 Revised September 2000

struct scm_f113_state {
	struct ctx ctx;
	bool dreg;
	int job;
};

SCM_F113 :: SCM_F113(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin4 << pin1.neg() << pin2 << pin3;

	state = (struct scm_f113_state *)
	    CTX_Get("f113", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F113 :: doit(void)
{
	const char *what;
	bool nxt = state->dreg;

	state->ctx.activations++;
	TRACE(
	    << state->job
	    << " clk " << pin1
	    << " k " << pin2
	    << " j " << pin3
	    << " pr " << pin4
	    << " q "
	    << state->dreg
	);
	if (state->job) {
		pin5 = AS(state->dreg);
		pin6 = AS(!state->dreg);
		state->job = 0;
	}

	if (IS_L(pin4)) {
		what = " PR ";
		nxt = true;
	} else if (!pin1.negedge()) {
		what = " ? ";
	} else if (IS_H(pin3) && IS_H(pin2)) {
		what = " inv ";
		nxt = !state->dreg;
	} else if (IS_L(pin3) && IS_H(pin2)) {
		what = " clr ";
		nxt = false;
	} else if (IS_H(pin3) && IS_L(pin2)) {
		what = " set ";
		nxt = true;
	} else {
		what = " nop ";
	}
	if (nxt != state->dreg) {
		state->job = 1;
		state->dreg = nxt;
		next_trigger(5, SC_NS);
	}
}
