#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F64.hh"

// 4-2-3-2-Input AND-OR-Invert Gate
// Fairchild DS009467 April 1988 Revised March 1999

struct scm_f64_state {
	struct ctx ctx;
};

SCM_F64 :: SCM_F64(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin6
		  << pin9 << pin10 << pin11 << pin12 << pin13;

	state = (struct scm_f64_state *)
	    CTX_Get("f64", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F64 :: doit(void)
{
	state->ctx.activations++;
	bool s = !(
	    (IS_H(pin2) && IS_H(pin3))
	    || (IS_H(pin1) && IS_H(pin13) && IS_H(pin12) && IS_H(pin11))
	    || (IS_H(pin4) && IS_H(pin5) && IS_H(pin6))
	    || (IS_H(pin9) && IS_H(pin10))
	);
	TRACE(
	    << pin2
	    << pin3
	    << ','
	    << pin9
	    << pin10
	    << ','
	    << pin4
	    << pin5
	    << pin6
	    << ','
	    << pin1
	    << pin11
	    << pin12
	    << pin13
	    << "|"
	    << s
	);
	pin8 = AS(s);
}
