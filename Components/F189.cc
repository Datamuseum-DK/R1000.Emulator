#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "F189.hh"

// 64-Bit Random Access Memory with 3-STATE Outputs
// Fairchild DS009493 April 1988 Revised January 2004

struct scm_f189_state {
	struct ctx ctx;
	bool ram[16][4];

};

void
SCM_F189 :: loadit(const char *arg)
{
	state = (struct scm_f189_state *)
	    CTX_Get("f189", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F189 :: doit(void)
{
	unsigned adr = 0;

	state->ctx.activations++;
	if (IS_H(pin1)) adr |= 1;
	if (IS_H(pin15)) adr |= 2;
	if (IS_H(pin14)) adr |= 4;
	if (IS_H(pin13)) adr |= 8;
	if (IS_L(pin2) && IS_L(pin3)) {
		state->ram[adr][0] = IS_L(pin4);
		state->ram[adr][1] = IS_L(pin6);
		state->ram[adr][2] = IS_L(pin18);
		state->ram[adr][3] = IS_L(pin12);
	}
	TRACE(
	    << pin1
	    << pin2
	    << pin3
	    << pin4
	    << pin6
	    << pin12
	    << pin13
	    << pin14
	    << pin15
	    << pin18
	    << "|"
	    << std::hex << adr
	    << " "
	    << state->ram[adr][0]
	    << state->ram[adr][1]
	    << state->ram[adr][2]
	    << state->ram[adr][3]
	);
	if (IS_L(pin2) && IS_H(pin3)) {
	    pin5 = AS(state->ram[adr][0]);
	    pin7 = AS(state->ram[adr][1]);
	    pin9 = AS(state->ram[adr][2]);
	    pin11 = AS(state->ram[adr][3]);
	} else {
	    pin5 = sc_logic_Z;
	    pin7 = sc_logic_Z;
	    pin9 = sc_logic_Z;
	    pin11 = sc_logic_Z;
	}
}
