#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "2149.hh"

struct scm_2149_state {
	struct ctx ctx;
	uint8_t ram[1024];
};

SCM_2149 :: SCM_2149(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin6 << pin7
	    << pin8 << pin10 << pin11 << pin12 << pin13 << pin14
	    << pin15 << pin16 << pin17;

	state = (struct scm_2149_state *)CTX_Get("2149", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	memset(state->ram, 0x0f, sizeof state->ram);
}

void
SCM_2149 :: doit(void)
{
	unsigned adr = 0, wrote = 0;

	state->ctx.activations++;

	if (IS_H(pin8)) {
		// TRACE(<< "z");
		pin14 = sc_logic_Z;
		pin13 = sc_logic_Z;
		pin12 = sc_logic_Z;
		pin11 = sc_logic_Z;
		next_trigger(pin8.negedge_event());
		return;
	}

	if (IS_H(pin15)) adr |= 1 << 0;
	if (IS_H(pin16)) adr |= 1 << 1;
	if (IS_H(pin17)) adr |= 1 << 2;
	if (IS_H(pin1)) adr |= 1 << 3;
	if (IS_H(pin2)) adr |= 1 << 4;
	if (IS_H(pin3)) adr |= 1 << 5;
	if (IS_H(pin4)) adr |= 1 << 6;
	if (IS_H(pin7)) adr |= 1 << 7;
	if (IS_H(pin6)) adr |= 1 << 8;
	if (IS_H(pin5)) adr |= 1 << 9;

	if (IS_L(pin10)) {
		pin14 = sc_logic_Z;
		pin13 = sc_logic_Z;
		pin12 = sc_logic_Z;
		pin11 = sc_logic_Z;
		state->ram[adr] = 0;
		if (IS_H(pin14)) state->ram[adr] |= (1<<3);
		if (IS_H(pin13)) state->ram[adr] |= (1<<2);
		if (IS_H(pin12)) state->ram[adr] |= (1<<1);
		if (IS_H(pin11)) state->ram[adr] |= (1<<0);
		wrote = 1;
	}
	if (wrote || (state->ctx.do_trace & 2)) {
		TRACE(
		    << " cs " << pin8
		    << " we " << pin10
		    << " a " << pin5 << pin6 << pin7
		    << pin4 << pin3 << pin2 << pin1
		    << pin17 << pin16 << pin15
		    << " dq "
		    << pin14
		    << pin13
		    << pin12
		    << pin11
		    << " | "
		    << std::hex << adr
		    << " "
		    << std::hex << (unsigned)(state->ram[adr])
		);
	}
	if (IS_H(pin10)) {
		pin14 = AS(state->ram[adr] & (1<<3));
		pin13 = AS(state->ram[adr] & (1<<2));
		pin12 = AS(state->ram[adr] & (1<<1));
		pin11 = AS(state->ram[adr] & (1<<0));
	}
}
