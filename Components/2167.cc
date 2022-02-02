#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "2167.hh"

// CMOS Static RAM 16K x 1-Bit
// Rensas DSC2981/08 February 2001

struct scm_2167_state {
	struct ctx ctx;
	bool ram[16384];

};

SCM_2167 :: SCM_2167(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin6 << pin7
	    << pin9 << pin11 << pin12
	    << pin13 << pin14 << pin15 << pin16 << pin17 << pin18 << pin19;

	state = (struct scm_2167_state *)
	    CTX_Get("2167", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_2167 :: doit(void)
{
	unsigned adr = 0;

	state->ctx.activations++;
	if (IS_H(pin1)) adr |= 1 << 13;
	if (IS_H(pin2)) adr |= 1 << 12;
	if (IS_H(pin3)) adr |= 1 << 11;
	if (IS_H(pin4)) adr |= 1 << 10;
	if (IS_H(pin5)) adr |= 1 << 9;
	if (IS_H(pin6)) adr |= 1 << 8;
	if (IS_H(pin7)) adr |= 1 << 7;
	if (IS_H(pin13)) adr |= 1 << 6;
	if (IS_H(pin14)) adr |= 1 << 5;
	if (IS_H(pin15)) adr |= 1 << 4;
	if (IS_H(pin16)) adr |= 1 << 3;
	if (IS_H(pin17)) adr |= 1 << 2;
	if (IS_H(pin18)) adr |= 1 << 1;
	if (IS_H(pin19)) adr |= 1 << 0;
	if (!IS_L(pin11)) {
		pin8 = sc_logic_Z;
	} else {
		if (IS_L(pin9))
			state->ram[adr] = IS_H(pin12);
		pin8 = AS(state->ram[adr]);
	}
	TRACE(
	    << " a "
	    << pin1
	    << pin2
	    << pin3
	    << pin4
	    << pin5
	    << pin6
	    << pin7
	    << pin13
	    << pin14
	    << pin15
	    << pin16
	    << pin17
	    << pin18
	    << pin19
	    << " d "
	    << pin12
	    << " w "
	    << pin9
	    << " oe "
	    << pin11
	    << " | "
	    << std::hex << adr
	    << " "
	    << state->ram[adr]
	);
}
