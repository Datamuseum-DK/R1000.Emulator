#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "P2K8R.hh"

struct scm_p2k8r_state {
	struct ctx ctx;
	uint8_t prom[2050];
	int last, nxt;
};

void
SCM_P2K8R :: loadit(const char *arg)
{
	state = (struct scm_p2k8r_state *)
	    CTX_Get("p2k8r", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	load_programmable(this->name(), state->prom, sizeof state->prom, arg);
	state->nxt = 2048;
}

void
SCM_P2K8R :: doit(void)
{
	const char *what = NULL;
	int adr = 0;

	state->ctx.activations++;
	if (state->nxt >= 0) {
		TRACE(
		    << " CK "
		    <<pin18
		    << " A "
		    <<pin21 <<pin22 <<pin23 <<pin1 <<pin2 <<pin3
		    <<pin4 <<pin5 <<pin6 <<pin7 <<pin8
		    << " OE_ "
		    <<pin19
		    << " MR_ "
		    <<pin20
		    << std::hex << " nxt "
		    << state->nxt
		    << " D "
		    << std::hex << (unsigned)state->prom[state->nxt]
		);
		pin17 = AS((state->prom[state->nxt] >> 7) & 1);
		pin16 = AS((state->prom[state->nxt] >> 6) & 1);
		pin15 = AS((state->prom[state->nxt] >> 5) & 1);
		pin14 = AS((state->prom[state->nxt] >> 4) & 1);
		pin13 = AS((state->prom[state->nxt] >> 3) & 1);
		pin11 = AS((state->prom[state->nxt] >> 2) & 1);
		pin10 = AS((state->prom[state->nxt] >> 1) & 1);
		pin9 = AS((state->prom[state->nxt] >> 0) & 1);
		state->last = state->nxt;
		state->nxt = -1;
	}
	if (IS_L(pin20)) {
		if (state->last != 2048)
			state->nxt = 2048;
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
		if (adr != state->last)
			state->nxt = adr;
		what = " CLK ";
	}
	assert(IS_L(pin19));
	if (state->nxt >= 0)
		next_trigger(5, SC_NS);
}
