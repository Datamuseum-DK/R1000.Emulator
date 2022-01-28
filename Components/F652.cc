#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F652.hh"

struct scm_F652_state {
	struct ctx ctx;
	unsigned areg;
	unsigned breg;
};

void
SCM_F652 :: loadit(const char *arg)
{
	state = (struct scm_F652_state *)CTX_Get("F652", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F652 :: doit_1(void)
{
	state->ctx.activations++;
	state->areg = 0;
	if (IS_H(pin4)) state->areg |= (1 << 7);
	if (IS_H(pin5)) state->areg |= (1 << 6);
	if (IS_H(pin6)) state->areg |= (1 << 5);
	if (IS_H(pin7)) state->areg |= (1 << 4);
	if (IS_H(pin8)) state->areg |= (1 << 3);
	if (IS_H(pin9)) state->areg |= (1 << 2);
	if (IS_H(pin10)) state->areg |= (1 << 1);
	if (IS_H(pin11)) state->areg |= (1 << 0);
	TRACE(
	    << " store A "
	    <<pin4 <<pin5 <<pin6 <<pin7
	    <<pin8 <<pin9 <<pin10 <<pin11
	);
}

void
SCM_F652 :: doit_2(void)
{
	state->ctx.activations++;
	state->breg = 0;
	if (IS_H(pin20)) state->breg |= (1 << 7);
	if (IS_H(pin19)) state->breg |= (1 << 6);
	if (IS_H(pin18)) state->breg |= (1 << 5);
	if (IS_H(pin17)) state->breg |= (1 << 4);
	if (IS_H(pin16)) state->breg |= (1 << 3);
	if (IS_H(pin15)) state->breg |= (1 << 2);
	if (IS_H(pin14)) state->breg |= (1 << 1);
	if (IS_H(pin13)) state->breg |= (1 << 0);

	TRACE(
	    << " store B "
	    <<pin20 <<pin19 <<pin18 <<pin17
	    <<pin16 <<pin15 <<pin14 <<pin13
	);
}

void
SCM_F652 :: doit_3(void)
{
	const char *what;

	state->ctx.activations++;
	if (IS_L(pin3)) {
		what = " Zb ";
		pin13 = sc_logic_Z;
		pin14 = sc_logic_Z;
		pin15 = sc_logic_Z;
		pin16 = sc_logic_Z;
		pin17 = sc_logic_Z;
		pin18 = sc_logic_Z;
		pin19 = sc_logic_Z;
		pin20 = sc_logic_Z;
	} else if (IS_H(pin2)) {
		what = " state->areg ";
		pin13 = AS(state->areg & 0x01);
		pin14 = AS(state->areg & 0x02);
		pin15 = AS(state->areg & 0x04);
		pin16 = AS(state->areg & 0x08);
		pin17 = AS(state->areg & 0x10);
		pin18 = AS(state->areg & 0x20);
		pin19 = AS(state->areg & 0x40);
		pin20 = AS(state->areg & 0x80);
	} else {
		what = " alive ";
		pin13 = AS(IS_H(pin11));
		pin14 = AS(IS_H(pin10));
		pin15 = AS(IS_H(pin9));
		pin16 = AS(IS_H(pin8));
		pin17 = AS(IS_H(pin7));
		pin18 = AS(IS_H(pin6));
		pin19 = AS(IS_H(pin5));
		pin20 = AS(IS_H(pin4));
	}
	TRACE(
	    << what
	    << " sab " << pin2
	    << " oeb " <<pin3
	    << " state->areg " << std::hex << state->areg
	    << " a " <<pin4 <<pin5 <<pin6 <<pin7
		<<pin8 <<pin9 <<pin10 <<pin11
	);
}

void
SCM_F652 :: doit_4(void)
{
	const char *what;

	state->ctx.activations++;
	if (IS_H(pin21)) {
		what = " Za ";
		pin11 = sc_logic_Z;
		pin10 = sc_logic_Z;
		pin9 = sc_logic_Z;
		pin8 = sc_logic_Z;
		pin7 = sc_logic_Z;
		pin6 = sc_logic_Z;
		pin5 = sc_logic_Z;
		pin4 = sc_logic_Z;
	} else if (IS_H(pin22)) {
		what = " state->breg ";
		pin11 = AS(state->breg & 0x01);
		pin10 = AS(state->breg & 0x02);
		pin9 = AS(state->breg & 0x04);
		pin8 = AS(state->breg & 0x08);
		pin7 = AS(state->breg & 0x10);
		pin6 = AS(state->breg & 0x20);
		pin5 = AS(state->breg & 0x40);
		pin4 = AS(state->breg & 0x80);
	} else {
		what = " blive ";
		pin11 = AS(IS_H(pin13));
		pin10 = AS(IS_H(pin14));
		pin9 = AS(IS_H(pin15));
		pin8 = AS(IS_H(pin16));
		pin7 = AS(IS_H(pin17));
		pin6 = AS(IS_H(pin18));
		pin5 = AS(IS_H(pin19));
		pin4 = AS(IS_H(pin20));
	}
	TRACE(
	    << what
	    << " sba " <<pin22
	    << " oeb " <<pin21
	    << " state->breg " << std::hex << state->breg
	    << " b " <<pin20 <<pin19 <<pin18 <<pin17
		<<pin16 <<pin15 <<pin14 <<pin13
	);
}
