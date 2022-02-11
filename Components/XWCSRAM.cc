#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "Components/XWCSRAM.hh"

struct scm_xwcsram_state {
	struct ctx ctx;
	uint8_t ram[16384];
};

SCM_XWCSRAM :: SCM_XWCSRAM(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive
	    << pin1 << pin3 << pin5 << pin7
	    << pin9 << pin11 << pin13 << pin15
	    << pin17 << pin18 << pin19 << pin20 << pin21 << pin22
	    << pin23 << pin24 << pin25 << pin26 << pin27 << pin28
	    << pin29 << pin30 << pin31;

	state = (struct scm_xwcsram_state *)CTX_Get("XWCSRAM", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XWCSRAM :: doit(void)
{
	unsigned adr = 0;

	state->ctx.activations++;

	if (IS_H(pin17))	adr |= 1 << 13;
	if (IS_H(pin18))	adr |= 1 << 12;
	if (IS_H(pin19))	adr |= 1 << 11;
	if (IS_H(pin20))	adr |= 1 << 10;
	if (IS_H(pin21))	adr |= 1 << 9;
	if (IS_H(pin22))	adr |= 1 << 8;
	if (IS_H(pin23))	adr |= 1 << 7;
	if (IS_H(pin24))	adr |= 1 << 6;
	if (IS_H(pin25))	adr |= 1 << 5;
	if (IS_H(pin26))	adr |= 1 << 4;
	if (IS_H(pin27))	adr |= 1 << 3;
	if (IS_H(pin28))	adr |= 1 << 2;
	if (IS_H(pin29))	adr |= 1 << 1;
	if (IS_H(pin30))	adr |= 1 << 0;

	if (IS_L(pin31)) {
		state->ram[adr] = 0;
		if (IS_H(pin1)) state->ram[adr] |= 0x80;
		if (IS_H(pin3)) state->ram[adr] |= 0x40;
		if (IS_H(pin5)) state->ram[adr] |= 0x20;
		if (IS_H(pin7)) state->ram[adr] |= 0x10;
		if (IS_H(pin9)) state->ram[adr] |= 0x08;
		if (IS_H(pin11)) state->ram[adr] |= 0x04;
		if (IS_H(pin13)) state->ram[adr] |= 0x02;
		if (IS_H(pin15)) state->ram[adr] |= 0x01;
		TRACE(
		    << " w a "
		    <<pin17 <<pin18 <<pin19 <<pin20 <<pin21 <<pin22 <<pin23
		    <<pin24 <<pin25 <<pin26 <<pin27 <<pin28 <<pin29 <<pin30
		    << " d "
		    <<pin1 <<pin3 <<pin5 <<pin7 <<pin9 <<pin11 <<pin13 <<pin15
		    << " we "
		    <<pin31
		    << " adr "
		    << std::hex << adr
		    << " data "
		    << std::hex << (unsigned)state->ram[adr]
		);
	} else {
		TRACE(
		    << " r a "
		    <<pin17 <<pin18 <<pin19 <<pin20 <<pin21 <<pin22 <<pin23
		    <<pin24 <<pin25 <<pin26 <<pin27 <<pin28 <<pin29 <<pin30
		    << " d "
		    <<AS(state->ram[adr] & 0x80)
		    <<AS(state->ram[adr] & 0x40)
		    <<AS(state->ram[adr] & 0x20)
		    <<AS(state->ram[adr] & 0x10)
		    <<AS(state->ram[adr] & 0x08)
		    <<AS(state->ram[adr] & 0x04)
		    <<AS(state->ram[adr] & 0x02)
		    <<AS(state->ram[adr] & 0x01)
		    << " we "
		    <<pin31
		    << " adr "
		    << std::hex << adr
		    << " data "
		    << std::hex << (unsigned)state->ram[adr]
		);
	}
	pin2 = AS(state->ram[adr] & 0x80);
	pin4 = AS(state->ram[adr] & 0x40);
	pin6 = AS(state->ram[adr] & 0x20);
	pin8 = AS(state->ram[adr] & 0x10);
	pin10 = AS(state->ram[adr] & 0x08);
	pin12 = AS(state->ram[adr] & 0x04);
	pin14 = AS(state->ram[adr] & 0x02);
	pin16 = AS(state->ram[adr] & 0x01);
}
