#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "2KX8.hh"

struct scm_2KX8_state {
	struct ctx ctx;
	uint8_t ram[1<<11];
};

void
SCM_2KX8 :: loadit(const char *arg)
{
	state = (struct scm_2KX8_state *)CTX_Get("2KX8", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_2KX8 :: doit(void)
{
	unsigned adr = 0;
	unsigned data = 0;

	state->ctx.activations++;

	TRACE(
	    << " a "
	    <<pin8 <<pin7 <<pin6 <<pin5 <<pin4 <<pin3
	    <<pin2 <<pin1 <<pin23 <<pin22 <<pin19
	    << " d "
	    <<pin9 <<pin10 <<pin11 <<pin13
	    <<pin14 <<pin15 <<pin16 <<pin17
	    << " cs " <<pin18
	    << " oe " <<pin20
	    << " we " << pin21
	);

	if (IS_H(pin18) || IS_H(pin20) || IS_L(pin21)) {
		pin9 = sc_logic_Z;
		pin10 = sc_logic_Z;
		pin11 = sc_logic_Z;
		pin13 = sc_logic_Z;
		pin14 = sc_logic_Z;
		pin15 = sc_logic_Z;
		pin16 = sc_logic_Z;
		pin17 = sc_logic_Z;
	}

	if (IS_H(pin19)) adr |= (1<<0);
	if (IS_H(pin22)) adr |= (1<<1);
	if (IS_H(pin23)) adr |= (1<<2);
	if (IS_H(pin1)) adr |= (1<<3);
	if (IS_H(pin2)) adr |= (1<<4);
	if (IS_H(pin3)) adr |= (1<<5);
	if (IS_H(pin4)) adr |= (1<<6);
	if (IS_H(pin5)) adr |= (1<<7);
	if (IS_H(pin6)) adr |= (1<<8);
	if (IS_H(pin7)) adr |= (1<<9);
	if (IS_H(pin8)) adr |= (1<<10);

	if (IS_L(pin18) && pin21.posedge()) {
		if (IS_H(pin17)) data |= (1<<0);
		if (IS_H(pin16)) data |= (1<<1);
		if (IS_H(pin15)) data |= (1<<2);
		if (IS_H(pin14)) data |= (1<<3);
		if (IS_H(pin13)) data |= (1<<4);
		if (IS_H(pin11)) data |= (1<<5);
		if (IS_H(pin10)) data |= (1<<6);
		if (IS_H(pin9)) data |= (1<<7);
		state->ram[adr] = data;
		TRACE(<< " write " << std::hex << adr << " " << std::hex << data);
	} else if (IS_L(pin20) && IS_H(pin21)) {
		data = state->ram[adr];
		pin17 = AS(data & (1<<0));
		pin16 = AS(data & (1<<1));
		pin15 = AS(data & (1<<2));
		pin14 = AS(data & (1<<3));
		pin13 = AS(data & (1<<4));
		pin11 = AS(data & (1<<5));
		pin10 = AS(data & (1<<6));
		pin9 = AS(data & (1<<7));
		TRACE(<< " read " << std::hex << adr << " " << std::hex << data);
	}
	return;
}
