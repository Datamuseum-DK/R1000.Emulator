#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F251.hh"

// 8-Input Multiplexer with 3-STATE Outputs
// Fairchild DS009504 April 1988 Revised September 2000

struct scm_f251_state {
	struct ctx ctx;
};

SCM_F251 :: SCM_F251(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2 << pin3 << pin4 << pin7 << pin9
		  << pin10 << pin11 << pin12 << pin13 << pin14 << pin15;

	state = (struct scm_f251_state *)
	    CTX_Get("f251", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F251 :: doit(void)
{
	unsigned adr = 0;
	bool s;

	state->ctx.activations++;

	if (IS_H(pin11)) adr |= 1;
	if (IS_H(pin10)) adr |= 2;
	if (IS_H(pin9)) adr |= 4;
	switch(adr) {
	case 0: s = IS_H(pin4); break;
	case 1: s = IS_H(pin3); break;
	case 2: s = IS_H(pin2); break;
	case 3: s = IS_H(pin1); break;
	case 4: s = IS_H(pin15); break;
	case 5: s = IS_H(pin14); break;
	case 6: s = IS_H(pin13); break;
	case 7: s = IS_H(pin12); break;
	}
	if (IS_H(pin7)) {
		pin5 = sc_logic_Z;
		pin6 = sc_logic_Z;
		next_trigger(pin7.negedge_event());
	} else {
		pin5 = AS(s);
		pin6 = AS(!s);
	}
	TRACE(
	    << " i "
	    <<pin4
	    <<pin3
	    <<pin2
	    <<pin1
	    <<pin15
	    <<pin14
	    <<pin13
	    <<pin12
	    << " s "
	    <<pin9
	    <<pin10
	    <<pin11
	    << " oe "
	    <<pin7
	    << " | "
	    << s
	);
}
