#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "25S10.hh"

// Four-Bit Shifter with Three-State Outputs
// Am25s10 03611B

struct scm_25s10_state {
	struct ctx ctx;
};

void
SCM_25S10 :: loadit(const char *arg)
{
	state = (struct scm_25s10_state *)
	    CTX_Get("25s10", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_25S10 :: doit(void)
{
	bool output[4];

	state->ctx.activations++;
	if (IS_L(pin9) && IS_L(pin10)) {
		output[0] = IS_H(pin4);
		output[1] = IS_H(pin5);
		output[2] = IS_H(pin6);
		output[3] = IS_H(pin7);
	} else if (IS_L(pin9) && IS_H(pin10)) {
		output[0] = IS_H(pin3);
		output[1] = IS_H(pin4);
		output[2] = IS_H(pin5);
		output[3] = IS_H(pin6);
	} else if (IS_H(pin9) && IS_L(pin10)) {
		output[0] = IS_H(pin2);
		output[1] = IS_H(pin3);
		output[2] = IS_H(pin4);
		output[3] = IS_H(pin5);
	} else {
		output[0] = IS_H(pin1);
		output[1] = IS_H(pin2);
		output[2] = IS_H(pin3);
		output[3] = IS_H(pin4);
	}
	TRACE(
	    << " s " <<pin9 <<pin10
	    << " oe_ " <<pin13
	    << " i "
	    <<pin1
	    <<pin2
	    <<pin3
	    <<pin4
	    <<pin5
	    <<pin6
	    <<pin7
	    << "|"
	    << output[0] << output[1] << output[2] << output[3]
	);
	if (IS_H(pin13)) {
		pin15 = sc_logic_Z;
		pin14 = sc_logic_Z;
		pin12 = sc_logic_Z;
		pin11 = sc_logic_Z;
	} else {
		pin15 = AS(output[0]);
		pin14 = AS(output[1]);
		pin12 = AS(output[2]);
		pin11 = AS(output[3]);
	}
}
