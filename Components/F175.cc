#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F175.hh"

// Quad D-Type Flip-Flop
// Fairchild DS009490 April 1988 Revised September 2000

struct scm_f175_state {
	struct ctx ctx;
	bool dreg[4];
	int job;
};

SCM_F175 :: SCM_F175(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin9.pos() << pin4 << pin5 << pin12 << pin13;

	state = (struct scm_f175_state *)
	    CTX_Get("f175", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	state->dreg[0] = false;
	state->dreg[1] = false;
	state->dreg[2] = false;
	state->dreg[3] = false;
	state->job = 1;
}

void
SCM_F175 :: doit(void)
{
	bool nxt[4];

	state->ctx.activations++;
	if (state->job || (state->ctx.do_trace & 2)) {
		TRACE(
		    << " job " << state->job
		    << " clr_ " << pin1
		    << " clk " << pin9
		    << " d "
		    << pin4
		    << pin5
		    << pin12
		    << pin13
		    << " |"
		    << " q "
		    << state->dreg[0]
		    << state->dreg[1]
		    << state->dreg[2]
		    << state->dreg[3]
		);
	}
	if (state->job) {
		pin2 = AS(state->dreg[0]);
		pin3 = AS(!state->dreg[0]);
		pin7 = AS(state->dreg[1]);
		pin6 = AS(!state->dreg[1]);
		pin10 = AS(state->dreg[2]);
		pin11 = AS(!state->dreg[2]);
		pin15 = AS(state->dreg[3]);
		pin14 = AS(!state->dreg[3]);
		state->job = 0;
	}
	nxt[0] = state->dreg[0];
	nxt[1] = state->dreg[1];
	nxt[2] = state->dreg[2];
	nxt[3] = state->dreg[3];
	if (IS_L(pin1)) {
		nxt[0] = false;
		nxt[1] = false;
		nxt[2] = false;
		nxt[3] = false;
	} else if (pin9.posedge()) {
		nxt[0] = IS_H(pin4);
		nxt[1] = IS_H(pin5);
		nxt[2] = IS_H(pin12);
		nxt[3] = IS_H(pin13);
	}
	if (
	    nxt[0] != state->dreg[0] ||
	    nxt[1] != state->dreg[1] ||
	    nxt[2] != state->dreg[2] ||
	    nxt[3] != state->dreg[3]) {
		state->dreg[0] = nxt[0];
		state->dreg[1] = nxt[1];
		state->dreg[2] = nxt[2];
		state->dreg[3] = nxt[3];
		state->job = 1;
		next_trigger(5, SC_NS);
	}
}
