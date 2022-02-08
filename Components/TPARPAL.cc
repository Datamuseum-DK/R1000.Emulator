// Generated from ../_Firmware/TPARGAL-02.BIN by gal_to_systemc.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "TPARPAL.hh"

struct scm_tparpal_state {
	struct ctx ctx;
	int job;
	int p12;
	int p13;
	int p14;
	int p15;
	int p16;
	int p18;
	int p19;
};
static const sc_logic outs[4] = {sc_logic_Z, sc_logic_Z, sc_logic_0, sc_logic_1};
static const char traces[5] = "ZZ01";

SCM_TPARPAL :: SCM_TPARPAL(sc_module_name nm, const char *arg)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin11 << pin2 << pin3 << pin4 << pin5 << pin6 << pin7 << pin8 << pin9;

	state = (struct scm_tparpal_state *)
	    CTX_Get("tparpal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_TPARPAL :: doit(void)
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
	assert(0 <= state->p18);
	assert(3 >= state->p18);
	assert(0 <= state->p19);
	assert(3 >= state->p19);
	if (state->job) {
		TRACE(
		    <<pin1
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
		    <<traces[state->p18]
		    <<traces[state->p19]
		);
		pin12 = outs[state->p12];
		pin13 = outs[state->p13];
		pin14 = outs[state->p14];
		pin15 = outs[state->p15];
		pin16 = outs[state->p16];
		pin18 = outs[state->p18];
		pin19 = outs[state->p19];
		state->job = 0;
	}
	bool p01 = IS_H(pin1);
	bool p02 = IS_H(pin2);
	bool p03 = IS_H(pin3);
	bool p04 = IS_H(pin4);
	bool p05 = IS_H(pin5);
	bool p06 = IS_H(pin6);
	bool p07 = IS_H(pin7);
	bool p08 = IS_H(pin8);
	bool p09 = IS_H(pin9);
	bool p11 = IS_H(pin11);
	int out19;
	int out18;
	int out16;
	int out15;
	int out14;
	int out13;
	int out12;
	out19 =
	    !((p04 && p05) ||
	    (p02 && (!p04)) ||
	    ((!p03) && (!p04)) ||
	    ((!p01) && (!p04) && (!p07) && (!p09)) ||
	    ((!p04) && (!p06) && (!p07) && (!p08) && (!p09)));
	out18 =
	    !(((!p03)) ||
	    ((!p04) && p07) ||
	    ((!p04) && p09));
	out16 =
	    !((p04) ||
	    ((!p02) && (!p03)) ||
	    ((!p02) && (!p01) && (!p07) && (!p09)) ||
	    ((!p02) && (!p06) && (!p07) && (!p08) && (!p09)));
	out15 =
	    !(((!p06)) ||
	    ((!p01) && p04) ||
	    ((!p01) && p03));
	out14 =
	    !(((!p08)) ||
	    ((!p01) && p04) ||
	    ((!p01) && p03));
	out13 =
	    !(((!p02) && p04 && (!p11)) ||
	    ((!p02) && (!p03) && (!p11)) ||
	    ((!p02) && (!p07) && (!p09) && (!p11)));
	out12 =
	    !(((!p03)) ||
	    (p04 && p05) ||
	    (p02 && (!p04)) ||
	    ((!p01) && (!p04) && (!p07) && (!p09)) ||
	    ((!p04) && (!p06) && (!p07) && (!p08) && (!p09)));

	assert(0 <= out19 && out19 <= 1);
	out19 += 2;
	assert(0 <= out18 && out18 <= 1);
	out18 += 2;
	assert(0 <= out16 && out16 <= 1);
	out16 += 2;
	assert(0 <= out15 && out15 <= 1);
	out15 += 2;
	assert(0 <= out14 && out14 <= 1);
	out14 += 2;
	assert(0 <= out13 && out13 <= 1);
	out13 += 2;
	assert(0 <= out12 && out12 <= 1);
	out12 += 2;

	if (
	    (out19 != state->p19) ||
	    (out18 != state->p18) ||
	    (out16 != state->p16) ||
	    (out15 != state->p15) ||
	    (out14 != state->p14) ||
	    (out13 != state->p13) ||
	    (out12 != state->p12)) {
		state->p19 = out19;
		state->p18 = out18;
		state->p16 = out16;
		state->p15 = out15;
		state->p14 = out14;
		state->p13 = out13;
		state->p12 = out12;
		state->job = 1;
		next_trigger(5, SC_NS);
	}
}
