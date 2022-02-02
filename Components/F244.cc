#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F244.hh"

// Octal buffers (3-state)
// Philips IC15 1994 Dec 05

struct scm_f244_state {
	struct ctx ctx;
};

SCM_F244 :: SCM_F244(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_THREAD(doit1);
	sensitive << pin1 << pin2 << pin4 << pin6 << pin8;
	SC_THREAD(doit2);
	sensitive << pin19 << pin11 << pin13 << pin15 << pin17;

	state = (struct scm_f244_state *)
	    CTX_Get("f244", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F244 :: doit1(void)
{
	/*
	 * IOC Pg 40 (TXCV) has F244 in bidirectional mode, and if both
	 * OE's are initially low, they will battle each other and the
	 * simulation will never get started.
	 * Refusing to participate the first nanosecond works around that.
	 */
	pin18 = sc_logic_Z;
	pin16 = sc_logic_Z;
	pin14 = sc_logic_Z;
	pin12 = sc_logic_Z;
	wait(1, SC_NS);
	while (1) {
		wait();
		state->ctx.activations++;
		if (IS_H(pin1)) {
			pin18 = sc_logic_Z;
			pin16 = sc_logic_Z;
			pin14 = sc_logic_Z;
			pin12 = sc_logic_Z;
		} else {
			pin18 = AS(IS_H(pin2));
			pin16 = AS(IS_H(pin4));
			pin14 = AS(IS_H(pin6));
			pin12 = AS(IS_H(pin8));
		}
		TRACE(
		    << " OE0_ " << pin1
		    << " D "
		    << pin2 << pin4 << pin6 << pin8
		);
	}
}

void
SCM_F244 :: doit2(void)
{
	pin3 = sc_logic_Z;
	pin5 = sc_logic_Z;
	pin7 = sc_logic_Z;
	pin9 = sc_logic_Z;
	wait(1, SC_NS);
	while (1) {
		wait();
		state->ctx.activations++;
		if (IS_H(pin19)) {
			pin3 = sc_logic_Z;
			pin5 = sc_logic_Z;
			pin7 = sc_logic_Z;
			pin9 = sc_logic_Z;
		} else {
			pin3 = AS(IS_H(pin17));
			pin5 = AS(IS_H(pin15));
			pin7 = AS(IS_H(pin13));
			pin9 = AS(IS_H(pin11));
		}
		TRACE(
		    << " OE1_ " << pin19
		    << " D "
		    << pin11 << pin13 << pin15 << pin17
		);
	}
}
