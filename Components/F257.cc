#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "F257.hh"

// Quad 2-Input Multiplexer with inverting 3-STATE Outputs
// TI SDLS148 - October 1976 - Revised March 1988

struct scm_f257_state {
	struct ctx ctx;
};

void
SCM_F257 :: loadit(const char *arg)
{
	state = (struct scm_f257_state *)
	    CTX_Get("f257", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F257 :: doit(void)
{
	bool output[4];

	state->ctx.activations++;
	if (IS_H(pin1)) {
		output[0] = IS_H(pin3);
		output[1] = IS_H(pin6);
		output[2] = IS_H(pin10);
		output[3] = IS_H(pin13);
	} else {
		output[0] = IS_H(pin2);
		output[1] = IS_H(pin5);
		output[2] = IS_H(pin11);
		output[3] = IS_H(pin14);
	}
	TRACE(
	    << " a " << pin2 << pin5 << pin11 << pin14
	    << " b " << pin3 << pin6 << pin10 << pin13
	    << " s " << pin1
	    << " oe " << pin15
	    << " | "
	    << output[0]
	    << output[1]
	    << output[2]
	    << output[3]
	);
	if (IS_H(pin15)) {
		pin4 = sc_logic_Z;
		pin7 = sc_logic_Z;
		pin9 = sc_logic_Z;
		pin12 = sc_logic_Z;
	} else {
		pin4 = AS(output[0]);
		pin7 = AS(output[1]);
		pin9 = AS(output[2]);
		pin12 = AS(output[3]);
	}
}
