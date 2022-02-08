// Generated from ../_Firmware/TAGAGAL-01.BIN by gal_to_systemc.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "TAGAPAL.hh"

struct scm_tagapal_state {
	struct ctx ctx;
	int job;
	int p16;
	int p17;
	int p18;
	int p19;
};
static const sc_logic outs[4] = {sc_logic_Z, sc_logic_Z, sc_logic_0, sc_logic_1};
static const char traces[5] = "ZZ01";

SCM_TAGAPAL :: SCM_TAGAPAL(sc_module_name nm, const char *arg)
{
	SC_METHOD(doit);
	sensitive << pin1.pos();

	state = (struct scm_tagapal_state *)
	    CTX_Get("tagapal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_TAGAPAL :: doit(void)
{

	state->ctx.activations++;

	assert(0 <= state->p16);
	assert(3 >= state->p16);
	assert(0 <= state->p17);
	assert(3 >= state->p17);
	assert(0 <= state->p18);
	assert(3 >= state->p18);
	assert(0 <= state->p19);
	assert(3 >= state->p19);
	if (state->job) {
		TRACE(
		    <<pin2
		    <<pin3
		    <<pin4
		    <<pin5
		    <<pin6
		    <<pin7
		    <<pin8
		    <<pin9
		    <<pin10
		    <<pin11
		    <<pin13
		    <<pin14
		    <<pin15
		    <<pin21
		    <<pin22
		    <<pin23
		    << " | "
		    <<traces[state->p16]
		    <<traces[state->p17]
		    <<traces[state->p18]
		    <<traces[state->p19]
		);
		pin16 = outs[state->p16];
		pin17 = outs[state->p17];
		pin18 = outs[state->p18];
		pin19 = outs[state->p19];
		state->job = 0;
	}
	bool p02 = IS_H(pin2);
	bool p03 = IS_H(pin3);
	bool p04 = IS_H(pin4);
	bool p05 = IS_H(pin5);
	bool p06 = IS_H(pin6);
	bool p07 = IS_H(pin7);
	bool p08 = IS_H(pin8);
	bool p09 = IS_H(pin9);
	bool p10 = IS_H(pin10);
	bool p11 = IS_H(pin11);
	bool p13 = IS_H(pin13);
	bool p14 = IS_H(pin14);
	bool p15 = IS_H(pin15);
	bool p16 = state->p16 % 2;
	bool p21 = IS_H(pin21);
	bool p22 = IS_H(pin22);
	bool p23 = IS_H(pin23);
	int out19 = state->p19 & 1;
	int out18 = state->p18 & 1;
	int out17 = state->p17 & 1;
	int out16 = state->p16 & 1;
	if (pin1.posedge()) {
		out19 =
		    (p22 && (!p06) && p11) ||
		    (p03 && p21 && (!p04) && p06 && (!p11)) ||
		    (p23 && p06 && p11) ||
		    ((!p02) && (!p03) && p21 && p06 && (!p11)) ||
		    (p06 && (!p08) && (!p11)) ||
		    (p03 && p04 && p08 && p09 && (!p11)) ||
		    (p02 && p04 && p08 && p09 && (!p11)) ||
		    (p02 && (!p03) && p08 && p09 && (!p11)) ||
		    ((!p02) && (!p03) && (!p21) && (!p04) && (!p05) && p09 && (!p11));
		out18 =
		    ((!p06) && p14 && p11) ||
		    (p03 && p21 && (!p04) && (!p06) && (!p11)) ||
		    (p06 && p15 && p11) ||
		    ((!p02) && (!p03) && p21 && (!p06) && (!p11)) ||
		    ((!p06) && (!p08) && (!p11)) ||
		    (p03 && p04 && p08 && (!p09) && p10 && (!p11)) ||
		    (p02 && p04 && p08 && (!p09) && p10 && (!p11)) ||
		    (p02 && (!p03) && p08 && (!p09) && p10 && (!p11)) ||
		    (p03 && p04 && p08 && p09 && (!p10) && (!p11)) ||
		    (p02 && p04 && p08 && p09 && (!p10) && (!p11)) ||
		    (p02 && (!p03) && p08 && p09 && (!p10) && (!p11)) ||
		    ((!p02) && (!p03) && (!p21) && (!p04) && (!p05) && (!p09) && p10 && (!p11)) ||
		    ((!p02) && (!p03) && (!p21) && (!p04) && (!p05) && p09 && (!p10) && (!p11));
		out17 =
		    (p02 && p03 && (!p04) && p08) ||
		    ((!p02) && p03 && (!p04) && (!p05) && p08) ||
		    ((!p02) && (!p03) && p05 && p08) ||
		    ((!p02) && (!p03) && p04 && (!p05) && p08);
		out16 =
		    (p02 && p03 && (!p04) && (!p06) && (!p07) && p08) ||
		    ((!p02) && (!p03) && p04 && (!p05) && (!p06) && (!p07) && p08) ||
		    (p06 && p16 && (!p13));
	}

	assert(0 <= out19 && out19 <= 1);
	out19 += 2;
	assert(0 <= out18 && out18 <= 1);
	out18 += 2;
	assert(0 <= out17 && out17 <= 1);
	out17 += 2;
	assert(0 <= out16 && out16 <= 1);
	out16 += 2;

	if (
	    (out19 != state->p19) ||
	    (out18 != state->p18) ||
	    (out17 != state->p17) ||
	    (out16 != state->p16)) {
		state->p19 = out19;
		state->p18 = out18;
		state->p17 = out17;
		state->p16 = out16;
		state->job = 1;
		next_trigger(5, SC_NS);
	}
}
