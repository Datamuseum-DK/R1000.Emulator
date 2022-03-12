#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS
#include "Components/XWCSRAM.hh"

struct scm_xwcsram_state {
	struct ctx ctx;
	uint8_t ram[16384];
};

SCM_XWCSRAM :: SCM_XWCSRAM(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive
	    << PIN_WE
	    << PIN_A0 << PIN_A1 << PIN_A2 << PIN_A3 << PIN_A4 << PIN_A5 << PIN_A6
	    << PIN_A7 << PIN_A8 << PIN_A9 << PIN_A10 << PIN_A11 << PIN_A12 << PIN_A13;

	state = (struct scm_xwcsram_state *)CTX_Get("XWCSRAM", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XWCSRAM :: doit(void)
{
	unsigned adr = 0;

	state->ctx.activations++;

	if (IS_H(PIN_A0))	adr |= 1 << 13;
	if (IS_H(PIN_A1))	adr |= 1 << 12;
	if (IS_H(PIN_A2))	adr |= 1 << 11;
	if (IS_H(PIN_A3))	adr |= 1 << 10;
	if (IS_H(PIN_A4))	adr |= 1 << 9;
	if (IS_H(PIN_A5))	adr |= 1 << 8;
	if (IS_H(PIN_A6))	adr |= 1 << 7;
	if (IS_H(PIN_A7))	adr |= 1 << 6;
	if (IS_H(PIN_A8))	adr |= 1 << 5;
	if (IS_H(PIN_A9))	adr |= 1 << 4;
	if (IS_H(PIN_A10))	adr |= 1 << 3;
	if (IS_H(PIN_A11))	adr |= 1 << 2;
	if (IS_H(PIN_A12))	adr |= 1 << 1;
	if (IS_H(PIN_A13))	adr |= 1 << 0;

	if (IS_L(PIN_WE)) {
		state->ram[adr] = 0;
		if (IS_H(PIN_D0)) state->ram[adr] |= 0x80;
		if (IS_H(PIN_D1)) state->ram[adr] |= 0x40;
		if (IS_H(PIN_D2)) state->ram[adr] |= 0x20;
		if (IS_H(PIN_D3)) state->ram[adr] |= 0x10;
		if (IS_H(PIN_D4)) state->ram[adr] |= 0x08;
		if (IS_H(PIN_D5)) state->ram[adr] |= 0x04;
		if (IS_H(PIN_D6)) state->ram[adr] |= 0x02;
		if (IS_H(PIN_D7)) state->ram[adr] |= 0x01;
		TRACE(
		    << " w a "
		    << PIN_A0 << PIN_A1 << PIN_A2 << PIN_A3 << PIN_A4 << PIN_A5 << PIN_A6
		    << PIN_A7 << PIN_A8 << PIN_A9 << PIN_A10 << PIN_A11 << PIN_A12 << PIN_A13
		    << " d "
		    << PIN_D0 << PIN_D1 << PIN_D2 << PIN_D3 << PIN_D4 << PIN_D5 << PIN_D6 << PIN_D7
		    << " we "
		    << PIN_WE
		    << " adr "
		    << std::hex << adr
		    << " data "
		    << std::hex << (unsigned)state->ram[adr]
		);
		next_trigger(
		    PIN_WE.posedge_event() |
		    PIN_D0.default_event() |
		    PIN_D1.default_event() |
		    PIN_D2.default_event() |
		    PIN_D3.default_event() |
		    PIN_D4.default_event() |
		    PIN_D5.default_event() |
		    PIN_D6.default_event() |
		    PIN_D7.default_event()
		);
	} else {
		TRACE(
		    << " r a "
		    << PIN_A0 << PIN_A1 << PIN_A2 << PIN_A3 << PIN_A4 << PIN_A5 << PIN_A6
		    << PIN_A7 << PIN_A8 << PIN_A9 << PIN_A10 << PIN_A11 << PIN_A12 << PIN_A13
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
		    << PIN_WE
		    << " adr "
		    << std::hex << adr
		    << " data "
		    << std::hex << (unsigned)state->ram[adr]
		);
	}
	PIN_Q0 = AS(state->ram[adr] & 0x80);
	PIN_Q1 = AS(state->ram[adr] & 0x40);
	PIN_Q2 = AS(state->ram[adr] & 0x20);
	PIN_Q3 = AS(state->ram[adr] & 0x10);
	PIN_Q4 = AS(state->ram[adr] & 0x08);
	PIN_Q5 = AS(state->ram[adr] & 0x04);
	PIN_Q6 = AS(state->ram[adr] & 0x02);
	PIN_Q7 = AS(state->ram[adr] & 0x01);
}
