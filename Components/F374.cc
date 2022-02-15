#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F374.hh"

// Octal D-Type Flip-Flop with 3-STATE Outputs
// Fairchild DS009524 May 1988 Revised September 2000

struct scm_f374_state {
	struct ctx ctx;
	int reg;
	int newreg;
};

SCM_F374 :: SCM_F374(sc_module_name nm, const char *arg) : sc_module(nm)
{
	state = (struct scm_f374_state *)
	    CTX_Get("f374", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);

	SC_METHOD(doit);
	sensitive << pin11.pos() << pin1;

	state->reg = 0;
	state->newreg = -2;
}

void
SCM_F374 :: doit(void)
{

	/* TEST_ABUS_PARITY.FIU requires a zero initial state */
	state->ctx.activations++;
	if (state->newreg == -2) {
		pin2 = sc_logic_Z;
		pin5 = sc_logic_Z;
		pin6 = sc_logic_Z;
		pin9 = sc_logic_Z;
		pin12 = sc_logic_Z;
		pin15 = sc_logic_Z;
		pin16 = sc_logic_Z;
		pin19 = sc_logic_Z;
		state->newreg = -1;
	}
	if (state->newreg >= 0) {
		state->reg = state->newreg;
		state->newreg = -1;
		TRACE(
		    << " upd " << " OE_ " << pin1
		    << " D "
		    << pin3 << pin4 << pin7 << pin8
		    << pin13 << pin14 << pin17 << pin18
		    << " | "
		    << std::hex << state->reg
		);
		if (IS_L(pin1)) {
			pin2 = AS(state->reg & (1<<7));
			pin5 = AS(state->reg & (1<<6));
			pin6 = AS(state->reg & (1<<5));
			pin9 = AS(state->reg & (1<<4));
			pin12 = AS(state->reg & (1<<3));
			pin15 = AS(state->reg & (1<<2));
			pin16 = AS(state->reg & (1<<1));
			pin19 = AS(state->reg & (1<<0));
		}
	}
	if (pin11.posedge()) {
		state->newreg = 0;
		if (IS_H(pin3)) state->newreg |= (1<<7);
		if (IS_H(pin4)) state->newreg |= (1<<6);
		if (IS_H(pin7)) state->newreg |= (1<<5);
		if (IS_H(pin8)) state->newreg |= (1<<4);
		if (IS_H(pin13)) state->newreg |= (1<<3);
		if (IS_H(pin14)) state->newreg |= (1<<2);
		if (IS_H(pin17)) state->newreg |= (1<<1);
		if (IS_H(pin18)) state->newreg |= (1<<0);
		if (state->newreg == state->reg) {
			state->newreg = -1;
		} else {
			next_trigger(1, SC_NS);
		}
	}
	if (pin1.posedge()) {
		if (state->ctx.do_trace & 2)
			TRACE(<< "Z");
		pin2 = sc_logic_Z;
		pin5 = sc_logic_Z;
		pin6 = sc_logic_Z;
		pin9 = sc_logic_Z;
		pin12 = sc_logic_Z;
		pin15 = sc_logic_Z;
		pin16 = sc_logic_Z;
		pin19 = sc_logic_Z;
	} else if (pin1.negedge()) {
		if (state->ctx.do_trace & 2)
			TRACE(<< "out " << std::hex << state->reg);
		pin2 = AS(state->reg & (1<<7));
		pin5 = AS(state->reg & (1<<6));
		pin6 = AS(state->reg & (1<<5));
		pin9 = AS(state->reg & (1<<4));
		pin12 = AS(state->reg & (1<<3));
		pin15 = AS(state->reg & (1<<2));
		pin16 = AS(state->reg & (1<<1));
		pin19 = AS(state->reg & (1<<0));
	}
}
