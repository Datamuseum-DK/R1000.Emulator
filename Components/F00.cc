#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F00.hh"

// (Quad) 2-input NAND

struct scm_f00_state {
	struct ctx ctx;
};

SCM_F00 :: SCM_F00(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2;

	state = (struct scm_f00_state *)
	    CTX_Get("f00", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F00 :: doit(void)
{

	state->ctx.activations++;
	bool s = !(IS_H(pin1) && IS_H(pin2));
	TRACE(
	    << pin1
	    << pin2
	    << "|"
	    << s
	);
	pin3 = AS(s);
	if (pin1 != sc_logic_1)
		next_trigger(pin1.posedge_event());
	else if (pin2 != sc_logic_1)
		next_trigger(pin2.posedge_event());
}
