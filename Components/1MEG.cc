#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "1MEG.hh"

struct scm_1MEG_state {
	struct ctx ctx;
	unsigned ras;
	unsigned cas;
	uint32_t bits[(1<<20)>>5];
};

SCM_1MEG :: SCM_1MEG(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin3.neg() << pin16 << pin2;
	state = (struct scm_1MEG_state *)CTX_Get("1MEG", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_1MEG :: doit(void)
{
	uint32_t adr = 0, data, mask;

	state->ctx.activations++;
	if (IS_H(pin5)) adr |= (1<<0);
	if (IS_H(pin6)) adr |= (1<<1);
	if (IS_H(pin7)) adr |= (1<<2);
	if (IS_H(pin8)) adr |= (1<<3);
	if (IS_H(pin10)) adr |= (1<<4);
	if (IS_H(pin11)) adr |= (1<<5);
	if (IS_H(pin12)) adr |= (1<<6);
	if (IS_H(pin13)) adr |= (1<<7);
	if (IS_H(pin14)) adr |= (1<<8);
	if (IS_H(pin15)) adr |= (1<<9);
	if (pin3.negedge())
		state->ras = adr;
	if (pin16.negedge()) {
		state->cas = adr;
		adr = (state->cas << 10) | state->ras;
		mask = adr & 0x1f;
		adr >>= 5;
		if (IS_L(pin2)) {
			pin17 = sc_logic_Z;
			if (IS_H(pin1))
				state->bits[adr] |= mask;
			else
				state->bits[adr] &= ~mask;
		} else {
			data = state->bits[adr] & mask;
			pin17 = AS(data);
		}
	}
	if (pin3.posedge() || pin16.posedge()) {
		pin17 = sc_logic_Z;
	}
	TRACE(
	    << " ras " << pin3
	    << " cas " << pin16
	    << " we " << pin2
	    << " a " << pin15 << pin14 << pin13 << pin12 << pin11 << pin10 << pin8 << pin7 << pin6 << pin5
	    << " d " << pin1
	    << " q " << pin17
	);
}

	//sc_in <sc_logic>	pin1;	// D (input)
	//sc_in <sc_logic>	pin2;	// WE_ (input)
	//sc_in <sc_logic>	pin3;	// RAS_ (input)
	//sc_in <sc_logic>	pin5;	// A10 (input)
	//sc_in <sc_logic>	pin6;	// A9 (input)
	//sc_in <sc_logic>	pin7;	// A8 (input)
	//sc_in <sc_logic>	pin8;	// A7 (input)
	//sc_in <sc_logic>	pin10;	// A6 (input)
	//sc_in <sc_logic>	pin11;	// A5 (input)
	//sc_in <sc_logic>	pin12;	// A4 (input)
	//sc_in <sc_logic>	pin13;	// A3 (input)
	//sc_in <sc_logic>	pin14;	// A2 (input)
	//sc_in <sc_logic>	pin15;	// A1 (input)
	//sc_in <sc_logic>	pin16;	// CAS_ (input)
	//sc_out <sc_logic>	pin17;	// Q (tri_state)

