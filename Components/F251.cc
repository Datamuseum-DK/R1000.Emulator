#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS

#include "F251.hh"

// 8-Input Multiplexer with 3-STATE Outputs
// Fairchild DS009504 April 1988 Revised September 2000

struct scm_f251_state {
	struct ctx ctx;
};

SCM_F251 :: SCM_F251(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << PIN_OE_ << PIN_S0 << PIN_S1 << PIN_S2
	    << PIN_A << PIN_B << PIN_C << PIN_D
	    << PIN_E << PIN_F << PIN_G << PIN_H;

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

	if (IS_H(PIN_S0)) adr |= 4;
	if (IS_H(PIN_S1)) adr |= 2;
	if (IS_H(PIN_S2)) adr |= 1;
	switch(adr) {
	case 0: s = IS_H(PIN_A); break;
	case 1: s = IS_H(PIN_B); break;
	case 2: s = IS_H(PIN_C); break;
	case 3: s = IS_H(PIN_D); break;
	case 4: s = IS_H(PIN_E); break;
	case 5: s = IS_H(PIN_F); break;
	case 6: s = IS_H(PIN_G); break;
	case 7: s = IS_H(PIN_H); break;
	}
	if (IS_H(PIN_OE_)) {
		PIN_Y = sc_logic_Z;
		PIN_Y_ = sc_logic_Z;
		next_trigger(PIN_OE_.negedge_event());
	} else {
		PIN_Y = AS(s);
		PIN_Y_ = AS(!s);
	}
	TRACE(
	    << " oe "
	    << PIN_OE_
	    << " i "
	    << PIN_A << PIN_B << PIN_C << PIN_D
	    << PIN_E << PIN_F << PIN_G << PIN_H
	    << " s "
	    << PIN_S0 << PIN_S1 << PIN_S2
	    << " | "
	    << s
	);
}
