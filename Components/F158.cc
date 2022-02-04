#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F158.hh"

// Quad 2-input data selector/multiplexer, inverting
// Philips IC15 1996 Mar 12

struct scm_f158_state {
	struct ctx ctx;
};

SCM_F158 :: SCM_F158(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2 << pin3 << pin5 << pin6
		  << pin10 << pin11 << pin13 << pin14 << pin15;

	state = (struct scm_f158_state *)
	    CTX_Get("f158", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F158 :: doit(void)
{
	bool output[4];

	state->ctx.activations++;
	if (IS_H(pin15)) {
		output[0] = true;
		output[1] = true;
		output[2] = true;
		output[3] = true;
		next_trigger(pin15.negedge_event());
	} else if (IS_H(pin1)) {
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
	    << " e " << pin15
	    << " | "
	    << output[0]
	    << output[1]
	    << output[2]
	    << output[3]
	);
	pin4 = AS(output[0]);
	pin7 = AS(output[1]);
	pin9 = AS(output[2]);
	pin12 = AS(output[3]);
}
