#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "16KX4.hh"

struct scm_16kx4_state {
	struct ctx ctx;
	uint8_t ram[1<<14];
};

SCM_16KX4 :: SCM_16KX4(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin6 << pin7
	    << pin8 << pin9 << pin17 << pin18 << pin19 << pin20 << pin21
            << pin10 << pin12;

	state = (struct scm_16kx4_state *)
	    CTX_Get("16kx4", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	memset(state->ram, 0xff, sizeof state->ram);
}

void
SCM_16KX4 :: doit(void)
{
	unsigned adr = 0;
	unsigned data;
	const char *what = NULL;

	state->ctx.activations++;
	if (IS_H(pin21)) adr |= (1<<0);
	if (IS_H(pin20)) adr |= (1<<1);
	if (IS_H(pin19)) adr |= (1<<2);
	if (IS_H(pin18)) adr |= (1<<3);
	if (IS_H(pin17)) adr |= (1<<4);
	if (IS_H(pin9)) adr |= (1<<5);
	if (IS_H(pin8)) adr |= (1<<6);
	if (IS_H(pin7)) adr |= (1<<7);
	if (IS_H(pin6)) adr |= (1<<8);
	if (IS_H(pin5)) adr |= (1<<9);
	if (IS_H(pin4)) adr |= (1<<10);
	if (IS_H(pin3)) adr |= (1<<11);
	if (IS_H(pin2)) adr |= (1<<12);
	if (IS_H(pin1)) adr |= (1<<13);
	data = state->ram[adr];
	if (pin10.posedge()) {
		pin16 = sc_logic_Z;
		pin15 = sc_logic_Z;
		pin14 = sc_logic_Z;
		pin13 = sc_logic_Z;
		what = " Z ";
		next_trigger(pin10.negedge_event());
	}
	if (pin10.negedge()) {
		if (IS_L(pin12)) {
			pin16 = sc_logic_Z;
			pin15 = sc_logic_Z;
			pin14 = sc_logic_Z;
			pin13 = sc_logic_Z;
			what = " Ew ";
		} else {
			what = " Er ";
		}
	}
	if (pin12.negedge()) {
		pin16 = sc_logic_Z;
		pin15 = sc_logic_Z;
		pin14 = sc_logic_Z;
		pin13 = sc_logic_Z;
		what = " w ";
	}
	if (pin12.posedge()) {
		data = 0;
		if (IS_H(pin13)) data |= (1 << 3);
		if (IS_H(pin14)) data |= (1 << 2);
		if (IS_H(pin15)) data |= (1 << 1);
		if (IS_H(pin16)) data |= (1 << 0);
		state->ram[adr] = data;
		what = " W ";
	}
	if (IS_L(pin10) && IS_H(pin12)) {
		pin13 = AS(data & (1<<3));
		pin14 = AS(data & (1<<2));
		pin15 = AS(data & (1<<1));
		pin16 = AS(data & (1<<0));
		if (state->ctx.do_trace & 2)
			what = " R ";
	}
	if (what != NULL) {
		TRACE(
		    << what
		    << " cs_ " << pin10
		    << " we_ " << pin12
		    << " a "
		    <<pin1 <<pin2 <<pin3 <<pin4 <<pin5 <<pin6 <<pin7
		    <<pin8 <<pin9 <<pin17 <<pin18 <<pin19 <<pin20 <<pin21
		    << " d "
		    <<pin13 <<pin14 <<pin15 <<pin16
		    << " A " << std::hex << adr
		    << " D " << std::hex << data
		);
	}
}
