#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS

#include "F299.hh"

// Octal Universal Shift/Storage Register with Common Parallel I/O Pins
// Fairchild DS009515 April 1988 Revised September 2000

struct scm_f299_state {
	struct ctx ctx;
	bool reg[8];
};

SCM_F299 :: SCM_F299(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << PIN_G1 << PIN_G2 << PIN_CLR << PIN_CLK.pos();

	state = (struct scm_f299_state *)
	    CTX_Get("f299", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	state->ctx.do_trace |= 3;
}

void
SCM_F299 :: doit(void)
{
	const char *what = NULL;
	unsigned mode = 0;

	state->ctx.activations++;

	if (IS_H(PIN_S0)) mode |= 2;
	if (IS_H(PIN_S1)) mode |= 1;

	if (mode == 0) {
		next_trigger(
		    PIN_S0.default_event() | PIN_S1.default_event() |
		    PIN_G1.default_event() | PIN_G2.default_event()
		);
	}

	if (IS_L(PIN_CLR)) {
		state->reg[0] = false;
		state->reg[1] = false;
		state->reg[2] = false;
		state->reg[3] = false;
		state->reg[4] = false;
		state->reg[5] = false;
		state->reg[6] = false;
		state->reg[7] = false;
		what = "clr ";
		next_trigger(PIN_CLR.posedge_event());
	} else if (PIN_CLK.posedge()) {
		switch (mode) {
		case 3:
			what = "load ";
			state->reg[0] = IS_H(PIN_DQ0);
			state->reg[1] = IS_H(PIN_DQ1);
			state->reg[2] = IS_H(PIN_DQ2);
			state->reg[3] = IS_H(PIN_DQ3);
			state->reg[4] = IS_H(PIN_DQ4);
			state->reg[5] = IS_H(PIN_DQ5);
			state->reg[6] = IS_H(PIN_DQ6);
			state->reg[7] = IS_H(PIN_DQ7);
			break;
		case 2:
			what = "<< ";
			state->reg[0] = state->reg[1];
			state->reg[1] = state->reg[2];
			state->reg[2] = state->reg[3];
			state->reg[3] = state->reg[4];
			state->reg[4] = state->reg[5];
			state->reg[5] = state->reg[6];
			state->reg[6] = state->reg[7];
			state->reg[7] = IS_H(PIN_LSI);
			break;
		case 1:
			what = ">> ";
			state->reg[7] = state->reg[6];
			state->reg[6] = state->reg[5];
			state->reg[5] = state->reg[4];
			state->reg[4] = state->reg[3];
			state->reg[3] = state->reg[2];
			state->reg[2] = state->reg[1];
			state->reg[1] = state->reg[0];
			state->reg[0] = IS_H(PIN_RSI);
			break;
		default:
			break;
		}
	}
	if ((IS_H(PIN_G1) || IS_H(PIN_G2)) || (mode == 3)) {
		if (what == NULL && (state->ctx.do_trace & 2))
			what = "Z ";
		PIN_DQ0 = sc_logic_Z;
		PIN_DQ1 = sc_logic_Z;
		PIN_DQ2 = sc_logic_Z;
		PIN_DQ3 = sc_logic_Z;
		PIN_DQ4 = sc_logic_Z;
		PIN_DQ5 = sc_logic_Z;
		PIN_DQ6 = sc_logic_Z;
		PIN_DQ7 = sc_logic_Z;
	} else {
		if (what == NULL && (state->ctx.do_trace & 2))
			what = "out ";
		PIN_DQ0 = AS(state->reg[0]);
		PIN_DQ1 = AS(state->reg[1]);
		PIN_DQ2 = AS(state->reg[2]);
		PIN_DQ3 = AS(state->reg[3]);
		PIN_DQ4 = AS(state->reg[4]);
		PIN_DQ5 = AS(state->reg[5]);
		PIN_DQ6 = AS(state->reg[6]);
		PIN_DQ7 = AS(state->reg[7]);
	}
	PIN_Q0 = AS(state->reg[0]);
	PIN_Q7 = AS(state->reg[7]);
	if (what != NULL) {
		TRACE(
		    << what
		    << "clk " << PIN_CLK.posedge()
		    << " s " << PIN_S0 << PIN_S1
		    << " g " << PIN_G1 << PIN_G2
		    << " mr " << PIN_CLR
		    << " rsi " << PIN_RSI
		    << " lsi " << PIN_LSI
		    << " dq "
		    << PIN_DQ0 << PIN_DQ1 << PIN_DQ2 << PIN_DQ3
		    << PIN_DQ4 << PIN_DQ5 << PIN_DQ6 << PIN_DQ7
		    << " | "
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
