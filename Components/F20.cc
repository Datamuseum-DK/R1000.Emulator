#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F20.hh"

// (Dual) 4-input NAND gate
// Philips IC15 1989 Mar 03

struct scm_f20_state {
	struct ctx ctx;
};

SCM_F20 :: SCM_F20(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2 << pin4 << pin5;

	state = (struct scm_f20_state *)
	    CTX_Get("f20", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F20 :: doit(void)
{

	state->ctx.activations++;
	bool s =
	    !(IS_H(pin1) && IS_H(pin2) && IS_H(pin4) && IS_H(pin5));
	TRACE(
	    << pin1
	    << pin2
	    << pin4
	    << pin5
	    << "|"
	    << s
	);
	pin6 = AS(s);
#if 0	// This does not work with the VAL ALUs OC-or'ing of A=B and VAL.CNAN3A
	if (pin1 != sc_logic_1)
		next_trigger(pin1.posedge_event());
	else if (pin2 != sc_logic_1)
		next_trigger(pin2.posedge_event());
	else if (pin4 != sc_logic_1)
		next_trigger(pin4.posedge_event());
	else if (pin5 != sc_logic_1)
		next_trigger(pin5.posedge_event());
#endif
}
