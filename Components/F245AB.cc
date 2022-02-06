#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F245AB.hh"

// Octal Bidirectional Transceiver with 3-STATE Outputs
// Fairchild Rev 1.4.0 January 2008

struct scm_f245ab_state {
	struct ctx ctx;
};

SCM_F245AB :: SCM_F245AB(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin6
		  << pin7 << pin8 << pin9 << pin11 << pin12 << pin13
		  << pin14 << pin15 << pin16 << pin17 << pin18 << pin19;

	state = (struct scm_f245ab_state *)
	    CTX_Get("f245ab", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F245AB :: doit(void)
{
	state->ctx.activations++;
	TRACE(
	    << " dir " << pin1
	    << " oe_ " << pin19
	    << " a " << pin2
	    <<pin3
	    <<pin4
	    <<pin5
	    <<pin6
	    <<pin7
	    <<pin8
	    <<pin9
	    << " b " << pin18
	    <<pin17
	    <<pin16
	    <<pin15
	    <<pin14
	    <<pin13
	    <<pin12
	    <<pin11
	);
	if (IS_H(pin19)) {
		pin2 = sc_logic_Z;
		pin3 = sc_logic_Z;
		pin4 = sc_logic_Z;
		pin5 = sc_logic_Z;
		pin6 = sc_logic_Z;
		pin7 = sc_logic_Z;
		pin8 = sc_logic_Z;
		pin9 = sc_logic_Z;
		pin11 = sc_logic_Z;
		pin12 = sc_logic_Z;
		pin13 = sc_logic_Z;
		pin14 = sc_logic_Z;
		pin15 = sc_logic_Z;
		pin16 = sc_logic_Z;
		pin17 = sc_logic_Z;
		pin18 = sc_logic_Z;
		next_trigger(pin19.negedge_event());
	} else if (IS_L(pin1)) {
		pin11 = sc_logic_Z;
		pin12 = sc_logic_Z;
		pin13 = sc_logic_Z;
		pin14 = sc_logic_Z;
		pin15 = sc_logic_Z;
		pin16 = sc_logic_Z;
		pin17 = sc_logic_Z;
		pin18 = sc_logic_Z;
		pin2 = AS(IS_H(pin18));
		pin3 = AS(IS_H(pin17));
		pin4 = AS(IS_H(pin16));
		pin5 = AS(IS_H(pin15));
		pin6 = AS(IS_H(pin14));
		pin7 = AS(IS_H(pin13));
		pin8 = AS(IS_H(pin12));
		pin9 = AS(IS_H(pin11));
	} else {
		pin2 = sc_logic_Z;
		pin3 = sc_logic_Z;
		pin4 = sc_logic_Z;
		pin5 = sc_logic_Z;
		pin6 = sc_logic_Z;
		pin7 = sc_logic_Z;
		pin8 = sc_logic_Z;
		pin9 = sc_logic_Z;
		pin11 = AS(IS_H(pin9));
		pin12 = AS(IS_H(pin8));
		pin13 = AS(IS_H(pin7));
		pin14 = AS(IS_H(pin6));
		pin15 = AS(IS_H(pin5));
		pin16 = AS(IS_H(pin4));
		pin17 = AS(IS_H(pin3));
		pin18 = AS(IS_H(pin2));
	}
}
