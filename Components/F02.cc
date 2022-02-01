#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F02.hh"

// (Quad) 2-input NOR

struct scm_f02_state {
	struct ctx ctx;
};

SCM_F02 :: SCM_F02(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2;

	state = (struct scm_f02_state *)
	    CTX_Get("f02", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F02 :: doit(void)
{
	state->ctx.activations++;
	bool s = !(IS_H(pin1) || IS_H(pin2));
	TRACE(
	    <<pin1
	    <<pin2
	    << "|"
	    << s
	);
	pin3 = AS(s);
}
