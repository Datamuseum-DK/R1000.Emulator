#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F51.hh"

// (Dual) 2-wide 3-input AND-OR-invert gate
// Philips IC15 1989 Mar 03

struct scm_f51_state {
	struct ctx ctx;
};

SCM_F51 :: SCM_F51(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2 << pin3
		  << pin4 << pin5 << pin6;

	state = (struct scm_f51_state *)
	    CTX_Get("f51", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F51 :: doit(void)
{
	state->ctx.activations++;
	bool s = !(
	    (IS_H(pin1) && IS_H(pin2) && IS_H(pin3))
	    ||
	    (IS_H(pin4) && IS_H(pin5) && IS_H(pin6))
	);
	TRACE(
	    << pin1
	    << pin2
	    << pin3
	    << pin4
	    << pin5
	    << pin6
	    << "|"
	    << s
	);
	pin7 = AS(s);
}
