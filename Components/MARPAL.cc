// Generated from ../_Firmware/MARGAL-02.BIN by gal_to_systemc.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "MARPAL.hh"

struct scm_marpal_state {
	struct ctx ctx;
	int job;
	int p12;
	int p13;
	int p14;
	int p15;
	int p16;
	int p17;
	int p18;
	int p19;
};
static const sc_logic outs[4] = {sc_logic_Z, sc_logic_Z, sc_logic_0, sc_logic_1};
static const char traces[5] = "ZZ01";

SCM_MARPAL :: SCM_MARPAL(sc_module_name nm, const char *arg)
{
	SC_METHOD(doit);
	sensitive << pin1.pos();

	state = (struct scm_marpal_state *)
	    CTX_Get("marpal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_MARPAL :: doit(void)
{

	state->ctx.activations++;

	assert(0 <= state->p12);
	assert(3 >= state->p12);
	assert(0 <= state->p13);
	assert(3 >= state->p13);
	assert(0 <= state->p14);
	assert(3 >= state->p14);
	assert(0 <= state->p15);
	assert(3 >= state->p15);
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
		    <<pin11
		    << " | "
		    <<traces[state->p12]
		    <<traces[state->p13]
		    <<traces[state->p14]
		    <<traces[state->p15]
		    <<traces[state->p16]
		    <<traces[state->p17]
		    <<traces[state->p18]
		    <<traces[state->p19]
		);
		pin12 = outs[state->p12];
		pin13 = outs[state->p13];
		pin14 = outs[state->p14];
		pin15 = outs[state->p15];
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
	bool p11 = IS_H(pin11);
	bool p12 = state->p12 % 2;
	bool p17 = state->p17 % 2;
	bool p19 = state->p19 % 2;
	int out19 = state->p19 & 1;
	int out18 = state->p18 & 1;
	int out17 = state->p17 & 1;
	int out16 = state->p16 & 1;
	int out15 = state->p15 & 1;
	int out14 = state->p14 & 1;
	int out13 = state->p13 & 1;
	int out12 = state->p12 & 1;
	if (pin1.posedge()) {
	out19 =
	    !((p04 && (!p06)) ||
		    (p02 && p04) ||
		    ((!p02) && (!p04)) ||
		    (p03 && p04 && p05) ||
		    (p03 && p04 && p07) ||
		    (p03 && p04 && p09 && p12) ||
		    (p03 && p04 && p17 && p08 && p09));
	out18 =
	    !((p04 && (!p06)) ||
		    (p04 && p05) ||
		    (p04 && p07) ||
		    ((!p03) && p04) ||
		    (p02 && p04) ||
		    ((!p02) && (!p04)) ||
		    (p04 && p09 && p12) ||
		    (p04 && p17 && p08 && p09));
	out17 =
	    !(((!p19)) ||
		    ((!p03) && p04) ||
		    (p02 && p04) ||
		    ((!p02) && (!p04)));
	out16 =
	    !((p02 && p04) ||
		    ((!p02) && (!p04)) ||
		    ((!p19) && (!p04)) ||
		    ((!p19) && p03) ||
		    ((!p03) && p04 && (!p06)));
	out15 =
	    !((p02 && p03 && p04));
	out14 =
	    !(((!p02) && (!p06)) ||
		    ((!p02) && (!p03)) ||
		    (p03 && p04 && p05) ||
		    (p03 && p04 && p07) ||
		    (p02 && p03 && p04) ||
		    (p03 && p04 && p09 && p12) ||
		    (p03 && p04 && p17 && p08 && p09));
	out13 =
	    !(((!p02) && (!p06)) ||
		    ((!p02) && p05) ||
		    ((!p02) && p07) ||
		    ((!p02) && (!p04)) ||
		    ((!p02) && (!p03)) ||
		    (p02 && p03 && p04) ||
		    ((!p02) && p09 && p12) ||
		    ((!p02) && p17 && p08 && p09));
	out12 =
	    !(((!p04) && (!p17) && (!p12)) ||
		    ((!p02) && (!p17) && (!p12)) ||
		    ((!p04) && p17 && (!p08)) ||
		    ((!p02) && p17 && (!p08)) ||
		    ((!p02) && p03 && (!p04) && (!p17)) ||
		    ((!p02) && (!p03) && p04 && (!p17)) ||
		    (p02 && p03 && p04 && (!p08)));
	}

	assert(0 <= out19 && out19 <= 1);
	if(!p11)
		out19 += 2;
	assert(0 <= out18 && out18 <= 1);
	if(!p11)
		out18 += 2;
	assert(0 <= out17 && out17 <= 1);
	if(!p11)
		out17 += 2;
	assert(0 <= out16 && out16 <= 1);
	if(!p11)
		out16 += 2;
	assert(0 <= out15 && out15 <= 1);
	if(!p11)
		out15 += 2;
	assert(0 <= out14 && out14 <= 1);
	if(!p11)
		out14 += 2;
	assert(0 <= out13 && out13 <= 1);
	if(!p11)
		out13 += 2;
	assert(0 <= out12 && out12 <= 1);
	if(!p11)
		out12 += 2;

	if (
	    (out19 != state->p19) ||
	    (out18 != state->p18) ||
	    (out17 != state->p17) ||
	    (out16 != state->p16) ||
	    (out15 != state->p15) ||
	    (out14 != state->p14) ||
	    (out13 != state->p13) ||
	    (out12 != state->p12)) {
		state->p19 = out19;
		state->p18 = out18;
		state->p17 = out17;
		state->p16 = out16;
		state->p15 = out15;
		state->p14 = out14;
		state->p13 = out13;
		state->p12 = out12;
		state->job = 1;
		next_trigger(5, SC_NS);
	}
}
