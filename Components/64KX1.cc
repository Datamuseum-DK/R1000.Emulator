#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "64KX1.hh"

struct scm_64kx1_state {
	struct ctx ctx;
	bool ram[1<<16];
};

void
SCM_64KX1 :: loadit(const char *arg)
{
	state = (struct scm_64kx1_state *)CTX_Get(
	    "64KX1", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_64KX1 :: doit(void)
{
	unsigned adr = 0;

	state->ctx.activations++;
	if (IS_H(pin21)) adr |= 1 << 0;
	if (IS_H(pin20)) adr |= 1 << 1;
	if (IS_H(pin19)) adr |= 1 << 2;
	if (IS_H(pin18)) adr |= 1 << 3;
	if (IS_H(pin17)) adr |= 1 << 4;
	if (IS_H(pin16)) adr |= 1 << 5;
	if (IS_H(pin15)) adr |= 1 << 6;
	if (IS_H(pin14)) adr |= 1 << 7;
	if (IS_H(pin8)) adr |= 1 << 8;
	if (IS_H(pin7)) adr |= 1 << 9;
	if (IS_H(pin6)) adr |= 1 << 10;
	if (IS_H(pin5)) adr |= 1 << 11;
	if (IS_H(pin4)) adr |= 1 << 12;
	if (IS_H(pin3)) adr |= 1 << 13;
	if (IS_H(pin2)) adr |= 1 << 14;
	if (IS_H(pin1)) adr |= 1 << 15;
	if (IS_L(pin12) && IS_L(pin10))
		state->ram[adr] = IS_H(pin13);
	TRACE(
	    <<pin1
	    <<pin2
	    <<pin3
	    <<pin4
	    <<pin5
	    <<pin6
	    <<pin7
	    <<pin8
	    <<pin14
	    <<pin15
	    <<pin16
	    <<pin17
	    <<pin18
	    <<pin19
	    <<pin20
	    <<pin21
	    << " CS# "
	    <<pin12
	    << " WE# "
	    <<pin10
	    << " D "
	    <<pin13
	    << " adr "
	    << std::hex << adr
	    << " data "
	    << state->ram[adr]
	);
	if (IS_L(pin12))
		pin9 = AS(state->ram[adr]);
	else
		pin9 = sc_logic_Z;
}
