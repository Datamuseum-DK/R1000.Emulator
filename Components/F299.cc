#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F299.hh"

// Octal Universal Shift/Storage Register with Common Parallel I/O Pins
// Fairchild DS009515 April 1988 Revised September 2000

struct scm_f299_state {
	struct ctx ctx;
	bool reg[8];
};

void
SCM_F299 :: loadit(const char *arg)
{
	state = (struct scm_f299_state *)
	    CTX_Get("f299", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F299 :: doit(void)
{
	const char *what = NULL;

	state->ctx.activations++;

	if (IS_L(pin9)) {
		state->reg[0] = false;
		state->reg[1] = false;
		state->reg[2] = false;
		state->reg[3] = false;
		state->reg[4] = false;
		state->reg[5] = false;
		state->reg[6] = false;
		state->reg[7] = false;
		what = " clr ";
	} else if (pin12.posedge()) {
		if (IS_H(pin19) && IS_H(pin1)) {
			what = " load ";
			state->reg[0] = IS_H(pin7);
			state->reg[1] = IS_H(pin13);
			state->reg[2] = IS_H(pin6);
			state->reg[3] = IS_H(pin14);
			state->reg[4] = IS_H(pin5);
			state->reg[5] = IS_H(pin15);
			state->reg[6] = IS_H(pin4);
			state->reg[7] = IS_H(pin16);
		} else if (IS_L(pin19) && IS_H(pin1)) {
			what = " right ";
			state->reg[7] = state->reg[6];
			state->reg[6] = state->reg[5];
			state->reg[5] = state->reg[4];
			state->reg[4] = state->reg[3];
			state->reg[3] = state->reg[2];
			state->reg[2] = state->reg[1];
			state->reg[1] = state->reg[0];
			state->reg[0] = IS_H(pin11);
		} else if (IS_H(pin19) && IS_L(pin1)) {
			what = " left ";
			state->reg[0] = state->reg[1];
			state->reg[1] = state->reg[2];
			state->reg[2] = state->reg[3];
			state->reg[3] = state->reg[4];
			state->reg[4] = state->reg[5];
			state->reg[5] = state->reg[6];
			state->reg[6] = state->reg[7];
			state->reg[7] = IS_H(pin18);
		}
	}
	if ((IS_H(pin19) && IS_H(pin1)) || IS_H(pin2) || IS_H(pin3)) {
		if (what == NULL && (state->ctx.do_trace & 2))
			what = " Z ";
		pin7 = sc_logic_Z;
		pin13 = sc_logic_Z;
		pin6 = sc_logic_Z;
		pin14 = sc_logic_Z;
		pin5 = sc_logic_Z;
		pin15 = sc_logic_Z;
		pin4 = sc_logic_Z;
		pin16 = sc_logic_Z;
	} else {
		if (what == NULL && (state->ctx.do_trace & 2))
			what = " out ";
		pin7 = AS(state->reg[0]);
		pin13 = AS(state->reg[1]);
		pin6 = AS(state->reg[2]);
		pin14 = AS(state->reg[3]);
		pin5 = AS(state->reg[4]);
		pin15 = AS(state->reg[5]);
		pin4 = AS(state->reg[6]);
		pin16 = AS(state->reg[7]);
	}
	pin8 = AS(state->reg[0]);
	pin17 = AS(state->reg[7]);
	if (what != NULL) {
		TRACE(
		    << what
		    << " clk" << pin12
		    << " s " << pin1 << pin19
		    << " oe " << pin2 << pin3
		    << " mr " << pin9
		    << " rsi " << pin11
		    << " lsi " << pin18
		    << " dq " << pin7 << pin13 << pin6 << pin14
		    << pin5 << pin15 << pin4 << pin16
		    << "|"
		    << state->reg[0]
		    << state->reg[1]
		    << state->reg[2]
		    << state->reg[3]
		    << state->reg[4]
		    << state->reg[5]
		    << state->reg[6]
		    << state->reg[7]
		);
	}
}
