#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F373.hh"

// Octal Transparent Latch with 3-STATE Outputs
// Fairchild DS009523 May 1988 Revised September 2000

struct scm_f373_state {
	struct ctx ctx;
	bool reg[8];
};

SCM_F373 :: SCM_F373(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin3 << pin4 << pin7 << pin8
		  << pin11 << pin13 << pin14 << pin17 << pin18;

	state = (struct scm_f373_state *)
	    CTX_Get("f373", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F373 :: doit(void)
{
	const char *what = NULL;

	state->ctx.activations++;
	if (IS_H(pin11)) {
		state->reg[0] = IS_H(pin3);
		state->reg[1] = IS_H(pin4);
		state->reg[2] = IS_H(pin7);
		state->reg[3] = IS_H(pin8);
		state->reg[4] = IS_H(pin13);
		state->reg[5] = IS_H(pin14);
		state->reg[6] = IS_H(pin17);
		state->reg[7] = IS_H(pin18);
		if (memcmp(oreg, state->reg, sizeof oreg)) {
			what = " new ";
			memcpy(oreg, state->reg, sizeof oreg);
		}
	}
	if (!IS_L(pin1)) {
		pin2 = sc_logic_Z;
		pin5 = sc_logic_Z;
		pin6 = sc_logic_Z;
		pin9 = sc_logic_Z;
		pin12 = sc_logic_Z;
		pin15 = sc_logic_Z;
		pin16 = sc_logic_Z;
		pin19 = sc_logic_Z;
	} else {
		pin2 = AS(state->reg[0]);
		pin5 = AS(state->reg[1]);
		pin6 = AS(state->reg[2]);
		pin9 = AS(state->reg[3]);
		pin12 = AS(state->reg[4]);
		pin15 = AS(state->reg[5]);
		pin16 = AS(state->reg[6]);
		pin19 = AS(state->reg[7]);
		if (what == NULL)
			what = " out ";
	}
	if (what != NULL) {
		TRACE(
		    << " oe " << pin1
		    << " le " << pin11
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
