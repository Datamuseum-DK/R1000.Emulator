#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F240.hh"

// Octal inverting buffers (3-state)
// Philips 2004 Feb 25

struct scm_f240_state {
	struct ctx ctx;
};

SCM_F240 :: SCM_F240(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_THREAD(doit1);
	sensitive << pin1 << pin2 << pin4 << pin6 << pin8;
	SC_THREAD(doit2);
	sensitive << pin19 << pin11 << pin13 << pin15 << pin17;

	state = (struct scm_f240_state *)
	    CTX_Get("f240", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F240 :: doit1(void)
{
	pin18 = sc_logic_Z;
	pin16 = sc_logic_Z;
	pin14 = sc_logic_Z;
	pin12 = sc_logic_Z;
	wait(1, SC_NS);
	while (1) {
		wait();
		state->ctx.activations++;
		wait(5, SC_NS);
		if (IS_H(pin1)) {
			pin18 = sc_logic_Z;
			pin16 = sc_logic_Z;
			pin14 = sc_logic_Z;
			pin12 = sc_logic_Z;
		} else {
			pin18 = AS(IS_L(pin2));
			pin16 = AS(IS_L(pin4));
			pin14 = AS(IS_L(pin6));
			pin12 = AS(IS_L(pin8));
		}
		TRACE(
		    << " OE0_ " << pin1
		    << " D "
		    << pin2 << pin4 << pin6 << pin8
		);
	}
}

void
SCM_F240 :: doit2(void)
{
	pin3 = sc_logic_Z;
	pin5 = sc_logic_Z;
	pin7 = sc_logic_Z;
	pin9 = sc_logic_Z;
	wait(1, SC_NS);
	while (1) {
		wait();
		state->ctx.activations++;
		wait(5, SC_NS);
		if (IS_H(pin19)) {
			pin3 = sc_logic_Z;
			pin5 = sc_logic_Z;
			pin7 = sc_logic_Z;
			pin9 = sc_logic_Z;
		} else {
			pin3 = AS(IS_L(pin17));
			pin5 = AS(IS_L(pin15));
			pin7 = AS(IS_L(pin13));
			pin9 = AS(IS_L(pin11));
		}
		TRACE(
		    << " OE1_ " << pin19
		    << " D "
		    << pin11 << pin13 << pin15 << pin17
		);
	}
}
