#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "F579.hh"

// 8-bit bidirectional binary counter (3-State)
// Fairchild DS009568 April 1988, Revised October 2000

struct scm_f579_state {
	struct ctx ctx;
	unsigned reg;
	bool z;
};

void
SCM_F579 :: loadit(const char *arg)
{
	state = (struct scm_f579_state *)
	    CTX_Get("f579", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F579 :: doit(void)
{
	const char *what = NULL;

	state->ctx.activations++;
	if (IS_H(pin20) && IS_H(pin19) && IS_L(pin12) && IS_L(pin13)) {
		// Parallel Load
		pin2 = sc_logic_Z;
		pin3 = sc_logic_Z;
		pin4 = sc_logic_Z;
		pin5 = sc_logic_Z;
		pin7 = sc_logic_Z;
		pin8 = sc_logic_Z;
		pin9 = sc_logic_Z;
		pin10 = sc_logic_Z;
	}

	if (IS_L(pin20)) {
		// Async reset
		state->reg = 0;
		what = " mr ";
	}

	if (pin1.posedge()) {
		if (IS_L(pin19)) {
			// Sync reset
			state->reg = 0;
			what = " sr ";
		} else if (IS_L(pin12) && IS_L(pin13)) {
			// Parallel Load
			state->reg = 0;
			if (IS_H(pin2)) state->reg |= (1<<0);
			if (IS_H(pin3)) state->reg |= (1<<1);
			if (IS_H(pin4)) state->reg |= (1<<2);
			if (IS_H(pin5)) state->reg |= (1<<3);
			if (IS_H(pin7)) state->reg |= (1<<4);
			if (IS_H(pin8)) state->reg |= (1<<5);
			if (IS_H(pin9)) state->reg |= (1<<6);
			if (IS_H(pin10)) state->reg |= (1<<7);
			what = " pl ";
		} else if (IS_H(pin18) || IS_H(pin17)) {
			// Hold
		} else if (IS_H(pin14)) {
			// Count Up
			state->reg += 1;
			what = " up ";
		} else {
			// Count Down
			state->reg += 255;
			what = " dn ";
		}
		state->reg &= 0xff;
	}

	if (IS_H(pin17))
		pin15 = AS(1);
	else if (IS_H(pin14))
		pin15 = AS(state->reg != 0xff);
	else
		pin15 = AS(state->reg != 0x00);

	if (IS_L(pin12) && IS_H(pin13) && IS_L(pin11)) {
		pin2 = AS(state->reg & (1<<0));
		pin3 = AS(state->reg & (1<<1));
		pin4 = AS(state->reg & (1<<2));
		pin5 = AS(state->reg & (1<<3));
		pin7 = AS(state->reg & (1<<4));
		pin8 = AS(state->reg & (1<<5));
		pin9 = AS(state->reg & (1<<6));
		pin10 = AS(state->reg & (1<<7));
		if (state->z && what == NULL)
		    what = " out ";
		state->z = false;
	} else {
		pin2 = sc_logic_Z;
		pin3 = sc_logic_Z;
		pin4 = sc_logic_Z;
		pin5 = sc_logic_Z;
		pin7 = sc_logic_Z;
		pin8 = sc_logic_Z;
		pin9 = sc_logic_Z;
		pin10 = sc_logic_Z;
		if (!state->z && what == NULL)
		    what = " Z ";
		state->z = true;
	}

	if (what == 0 && (state->ctx.do_trace & 2))
		what = " - ";
	if (what != NULL) {
		TRACE(
		    << what
		    << " cp " << pin1
		    << " io "
		    <<pin10 <<pin9 <<pin8 <<pin7 <<pin5 <<pin4 <<pin3 <<pin2
		    << " oe_ " <<pin11
		    << " cs_ " <<pin12
		    << " pe_ " <<pin13
		    << " u/d_ " <<pin14
		    << " cet_ " <<pin17
		    << " cep_ " <<pin18
		    << " sr_ " <<pin19
		    << " mr_ " <<pin20
		    << " cnt 0x" << std::hex << state->reg
		);
	}
}
