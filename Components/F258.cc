#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "F258.hh"

// Quad 2-Input Multiplexer with 3-STATE Outputs
// Fairchild DS009508 April 1988 Revised September 2000

struct scm_f258_state {
	struct ctx ctx;
};

void
SCM_F258 :: loadit(const char *arg)
{
	state = (struct scm_f258_state *)
	    CTX_Get("f258", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F258 :: doit(void)
{
	bool output[4];

	state->ctx.activations++;
	if (IS_H(pin1)) {
		output[0] = IS_L(pin3);
		output[1] = IS_L(pin6);
		output[2] = IS_L(pin10);
		output[3] = IS_L(pin13);
	} else {
		output[0] = IS_L(pin2);
		output[1] = IS_L(pin5);
		output[2] = IS_L(pin11);
		output[3] = IS_L(pin14);
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
	if (!IS_L(pin15)) {
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
