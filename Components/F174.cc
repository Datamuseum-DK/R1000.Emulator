#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F174.hh"

// Hex D-Type Flip-Flop with Master Reset
// Fairchild DS009489 April 1988 Revised September 2000

struct scm_f174_state {
	struct ctx ctx;
	bool dreg[6];
	int job;
};

SCM_F174 :: SCM_F174(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin9.pos();

	state = (struct scm_f174_state *)
	    CTX_Get("f174", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	state->job = 1;
}

void
SCM_F174 :: doit(void)
{
	const char *what;
	bool nxt[6];

	state->ctx.activations++;
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
	if (state->job) {
		pin2 = AS(state->dreg[0]);
		pin5 = AS(state->dreg[1]);
		pin7 = AS(state->dreg[2]);
		pin10 = AS(state->dreg[3]);
		pin12 = AS(state->dreg[4]);
		pin15 = AS(state->dreg[5]);
		state->job = 0;
	}
	if (IS_L(pin1)) {
		nxt[0] = false;
		nxt[1] = false;
		nxt[2] = false;
		nxt[3] = false;
		nxt[4] = false;
		nxt[5] = false;
		what = " CLR ";
	} else if (pin9.posedge()) {
		nxt[0] = IS_H(pin3);
		nxt[1] = IS_H(pin4);
		nxt[2] = IS_H(pin6);
		nxt[3] = IS_H(pin11);
		nxt[4] = IS_H(pin13);
		nxt[5] = IS_H(pin14);
		what = " CLK ";
	} else {
		nxt[0] = state->dreg[0];
		nxt[1] = state->dreg[1];
		nxt[2] = state->dreg[2];
		nxt[3] = state->dreg[3];
		nxt[4] = state->dreg[4];
		nxt[5] = state->dreg[5];
		what = " ??? ";
	}
	if (
	    nxt[0] != state->dreg[0] ||
	    nxt[1] != state->dreg[1] ||
	    nxt[2] != state->dreg[2] ||
	    nxt[3] != state->dreg[3] ||
	    nxt[4] != state->dreg[4] ||
	    nxt[5] != state->dreg[5]
	) {
		state->job = 1;
		state->dreg[0] = nxt[0];
		state->dreg[1] = nxt[1];
		state->dreg[2] = nxt[2];
		state->dreg[3] = nxt[3];
		state->dreg[4] = nxt[4];
		state->dreg[5] = nxt[5];
		next_trigger(5, SC_NS);
	}
}
