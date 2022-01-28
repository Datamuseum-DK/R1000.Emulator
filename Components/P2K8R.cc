#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "P2K8R.hh"

struct scm_p2k8r_state {
	struct ctx ctx;
	uint8_t prom[2050];
	bool reg[8];
	uint8_t last;
};

void
SCM_P2K8R :: loadit(const char *arg)
{
	state = (struct scm_p2k8r_state *)
	    CTX_Get("p2k8r", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	load_programmable(this->name(), state->prom, sizeof state->prom, arg);
	state->reg[0] = (state->prom[2048] >> 7) & 1;
	state->reg[1] = (state->prom[2048] >> 6) & 1;
	state->reg[2] = (state->prom[2048] >> 5) & 1;
	state->reg[3] = (state->prom[2048] >> 4) & 1;
	state->reg[4] = (state->prom[2048] >> 3) & 1;
	state->reg[5] = (state->prom[2048] >> 2) & 1;
	state->reg[6] = (state->prom[2048] >> 1) & 1;
	state->reg[7] = (state->prom[2048] >> 0) & 1;
}

void
SCM_P2K8R :: doit(void)
{
	const char *what = NULL;
	unsigned adr = 0, dotrace = 0;

	state->ctx.activations++;
	if (IS_L(pin20)) {
		state->reg[0] = (state->prom[2048] >> 7) & 1;
		state->reg[1] = (state->prom[2048] >> 6) & 1;
		state->reg[2] = (state->prom[2048] >> 5) & 1;
		state->reg[3] = (state->prom[2048] >> 4) & 1;
		state->reg[4] = (state->prom[2048] >> 3) & 1;
		state->reg[5] = (state->prom[2048] >> 2) & 1;
		state->reg[6] = (state->prom[2048] >> 1) & 1;
		state->reg[7] = (state->prom[2048] >> 0) & 1;
		what = " MR ";
	} else if (pin18.posedge()) {
		if (IS_H(pin8)) adr |= 1 << 0;
		if (IS_H(pin7)) adr |= 1 << 1;
		if (IS_H(pin6)) adr |= 1 << 2;
		if (IS_H(pin5)) adr |= 1 << 3;
		if (IS_H(pin4)) adr |= 1 << 4;
		if (IS_H(pin3)) adr |= 1 << 5;
		if (IS_H(pin2)) adr |= 1 << 6;
		if (IS_H(pin1)) adr |= 1 << 7;
		if (IS_H(pin23)) adr |= 1 << 8;
		if (IS_H(pin22)) adr |= 1 << 9;
		if (IS_H(pin21)) adr |= 1 << 10;
		uint8_t data = state->prom[adr];
		state->reg[0] = (data >> 7) & 1;
		state->reg[1] = (data >> 6) & 1;
		state->reg[2] = (data >> 5) & 1;
		state->reg[3] = (data >> 4) & 1;
		state->reg[4] = (data >> 3) & 1;
		state->reg[5] = (data >> 2) & 1;
		state->reg[6] = (data >> 1) & 1;
		state->reg[7] = (data >> 0) & 1;
		what = " CLK ";
		if (data != state->last) {
			state->last = data;
			dotrace = 1;
		}
	}
	if (IS_H(pin19)) {
		pin17 = sc_logic_Z;
		pin16 = sc_logic_Z;
		pin15 = sc_logic_Z;
		pin14 = sc_logic_Z;
		pin13 = sc_logic_Z;
		pin11 = sc_logic_Z;
		pin10 = sc_logic_Z;
		pin9 = sc_logic_Z;
	} else {
		pin17 = AS(state->reg[0]);
		pin16 = AS(state->reg[1]);
		pin15 = AS(state->reg[2]);
		pin14 = AS(state->reg[3]);
		pin13 = AS(state->reg[4]);
		pin11 = AS(state->reg[5]);
		pin10 = AS(state->reg[6]);
		pin9 = AS(state->reg[7]);
	}
	if ((state->ctx.do_trace & 2) || dotrace) {
		TRACE(
		    << what
		    << " CK "
		    <<pin18
		    << " A "
		    <<pin21 <<pin22 <<pin23 <<pin1 <<pin2 <<pin3
		    <<pin4 <<pin5 <<pin6 <<pin7 <<pin8
		    << " OE_ "
		    <<pin19
		    << " MR_ "
		    <<pin20
		    << " ADR "
		    << adr
		    << " D "
		    <<pin17 <<pin16 <<pin15 <<pin14
		    <<pin13 <<pin11 <<pin10 <<pin9
		);
	}
	assert (IS_L(pin19));
}
