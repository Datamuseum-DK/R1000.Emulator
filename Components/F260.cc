#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F260.hh"

// (Dual) 5-input NOR gate
// Philips IC15 1988 Nov 29

struct scm_f260_state {
	struct ctx ctx;
};

SCM_F260 :: SCM_F260(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2 << pin3 << pin12 << pin13;

	state = (struct scm_f260_state *)
	    CTX_Get("f260", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F260 :: doit(void)
{

	state->ctx.activations++;
	bool s = !(IS_H(pin1) || IS_H(pin2) || IS_H(pin3)
	    || IS_H(pin12) || IS_H(pin13));
	TRACE(
	    << pin12
	    << pin13
	    << pin1
	    << pin2
	    << pin3
	    << " | "
	    << s
	);
	pin5 = AS(s);
}
