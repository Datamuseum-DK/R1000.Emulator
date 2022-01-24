#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "r1000sc_priv.h"
#include "PBxxx.hh"

// Signetics 82S123A, product specification, June 6 1985
// 32x8 PROM

struct scm_pbxxx_state {
	struct ctx ctx;
	uint8_t prom[32];
};

void
SCM_PBXXX :: loadit(const char *arg)
{
	state = (struct scm_pbxxx_state *)
	    CTX_Get("pbxxx", this->name(), sizeof *state);
	load_programmable(this->name(), state->prom, sizeof state->prom, arg);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_PBXXX :: doit(void)
{
	unsigned adr = 0, data = 0;

	state->ctx.activations++;
	if (IS_H(pin15)) {
		pin1 = sc_logic_Z;
		pin2 = sc_logic_Z;
		pin3 = sc_logic_Z;
		pin4 = sc_logic_Z;
		pin5 = sc_logic_Z;
		pin6 = sc_logic_Z;
		pin7 = sc_logic_Z;
		pin9 = sc_logic_Z;
		TRACE(<< " Z");
	} else {
		if (IS_H(pin10)) adr |= (1 << 0);
		if (IS_H(pin11)) adr |= (1 << 1);
		if (IS_H(pin12)) adr |= (1 << 2);
		if (IS_H(pin13)) adr |= (1 << 3);
		if (IS_H(pin14)) adr |= (1 << 4);
		data = state->prom[adr];
		pin1 = AS(data & (1 << 0));
		pin2 = AS(data & (1 << 1));
		pin3 = AS(data & (1 << 2));
		pin4 = AS(data & (1 << 3));
		pin5 = AS(data & (1 << 4));
		pin6 = AS(data & (1 << 5));
		pin7 = AS(data & (1 << 6));
		pin9 = AS(data & (1 << 7));
		TRACE(
		    << " a " << pin14 << pin13 << pin12 << pin11 << pin10
		    << " = 0x" << std::hex << adr
		    << " d 0x" << std::hex << data
		);
	}
}
