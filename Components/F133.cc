#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F133.hh"

// 13-input NAND gate
// Philips IC15 1993 Jul 02

struct scm_f133_state {
	struct ctx ctx;
};

SCM_F133 :: SCM_F133(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin6
		  << pin7 << pin10 << pin11 << pin12 << pin13
		  << pin14 << pin15;

	state = (struct scm_f133_state *)
	    CTX_Get("f133", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F133 :: doit(void)
{
	state->ctx.activations++;
	bool s = !(IS_H(pin1) && IS_H(pin2) && IS_H(pin3) && IS_H(pin4)
	    && IS_H(pin5) && IS_H(pin6) && IS_H(pin7) && IS_H(pin10)
	    && IS_H(pin11) && IS_H(pin12) && IS_H(pin13) && IS_H(pin14)
	    && IS_H(pin15));
	TRACE(
	    << pin10
	    << pin11
	    << pin12
	    << pin13
	    << pin14
	    << pin15
	    << pin1
	    << pin2
	    << pin3
	    << pin4
	    << pin5
	    << pin6
	    << pin7
	    << "|"
	    << s
	);
	pin8 = AS(s);
}
