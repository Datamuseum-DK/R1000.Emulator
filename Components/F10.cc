#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F10.hh"

// (Triple) 3-input NAND gate

struct scm_f10_state {
	struct ctx ctx;
};

SCM_F10 :: SCM_F10(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2 << pin3;

	state = (struct scm_f10_state *)
	    CTX_Get("f10", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F10 :: doit(void)
{
	state->ctx.activations++;
	bool s = !(IS_H(pin1) && IS_H(pin2) && IS_H(pin3));
	if ((state->ctx.do_trace & 2) || s != ostate) {
		TRACE(
		    << pin1
		    << pin2
		    << pin3
		    << "|"
		    << s
		);
		ostate = s;
	}
	pin4 = AS(s);
	if (pin1 != sc_logic_1)
		next_trigger(pin1.posedge_event());
	else if (pin2 != sc_logic_1)
		next_trigger(pin2.posedge_event());
	else if (pin3 != sc_logic_1)
		next_trigger(pin3.posedge_event());
}
