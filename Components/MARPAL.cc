// Generated from ../_Firmware/MARGAL-02.BIN by gal16v8.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "MARPAL.hh"

struct scm_marpal_state {
	struct ctx ctx;
	bool p12;
	bool p13;
	bool p14;
	bool p15;
	bool p16;
	bool p17;
	bool p18;
	bool p19;
};

void
SCM_MARPAL :: loadit(const char *arg)
{
	state = (struct scm_marpal_state *)
	    CTX_Get("marpal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_MARPAL :: doit(void)
{

	state->ctx.activations++;

	bool p02 = IS_H(pin2);
	bool p03 = IS_H(pin3);
	bool p04 = IS_H(pin4);
	bool p05 = IS_H(pin5);
	bool p06 = IS_H(pin6);
	bool p07 = IS_H(pin7);
	bool p08 = IS_H(pin8);
	bool p09 = IS_H(pin9);
	bool p11 = IS_H(pin11);
	bool p12 = state->p12;
	bool p17 = state->p17;
	bool p19 = state->p19;
	bool dotrace = false;
	if (pin1.posedge()) {
		dotrace = true;
		state->p19 =
		    !(((!p06) && p04) ||
		    (p02 && p04) ||
		    ((!p02) && (!p04)) ||
		    (p03 && p04 && p05) ||
		    (p03 && p04 && p07) ||
		    (p03 && p04 && p09 && p12) ||
		    (p03 && p04 && p08 && p09 && p17));
		state->p18 =
		    !(((!p06) && p04) ||
		    (p04 && p05) ||
		    (p04 && p07) ||
		    ((!p03) && p04) ||
		    (p02 && p04) ||
		    ((!p02) && (!p04)) ||
		    (p04 && p09 && p12) ||
		    (p04 && p08 && p09 && p17));
		state->p17 =
		    !(((!p19)) ||
		    ((!p03) && p04) ||
		    (p02 && p04) ||
		    ((!p02) && (!p04)));
		state->p16 =
		    !((p02 && p04) ||
		    ((!p02) && (!p04)) ||
		    ((!p04) && (!p19)) ||
		    ((!p19) && p03) ||
		    ((!p03) && (!p06) && p04));
		state->p15 =
		    !((p02 && p03 && p04));
		state->p14 =
		    !(((!p02) && (!p06)) ||
		    ((!p02) && (!p03)) ||
		    (p03 && p04 && p05) ||
		    (p03 && p04 && p07) ||
		    (p02 && p03 && p04) ||
		    (p03 && p04 && p09 && p12) ||
		    (p03 && p04 && p08 && p09 && p17));
		state->p13 =
		    !(((!p02) && (!p06)) ||
		    ((!p02) && p05) ||
		    ((!p02) && p07) ||
		    ((!p02) && (!p04)) ||
		    ((!p02) && (!p03)) ||
		    (p02 && p03 && p04) ||
		    ((!p02) && p09 && p12) ||
		    ((!p02) && p08 && p09 && p17));
		state->p12 =
		    !(((!p04) && (!p12) && (!p17)) ||
		    ((!p02) && (!p12) && (!p17)) ||
		    ((!p04) && (!p08) && p17) ||
		    ((!p02) && (!p08) && p17) ||
		    ((!p02) && (!p04) && (!p17) && p03) ||
		    ((!p02) && (!p03) && (!p17) && p04) ||
		    ((!p08) && p02 && p03 && p04));
	}

	char out19;
	if(!p11) {
		out19 = state->p19 ? '1' : '0';
		pin19 = AS(state->p19);
	} else {
		out19 = 'Z';
		pin19 = sc_logic_Z;
	}
	char out18;
	if(!p11) {
		out18 = state->p18 ? '1' : '0';
		pin18 = AS(state->p18);
	} else {
		out18 = 'Z';
		pin18 = sc_logic_Z;
	}
	char out17;
	if(!p11) {
		out17 = state->p17 ? '1' : '0';
		pin17 = AS(state->p17);
	} else {
		out17 = 'Z';
		pin17 = sc_logic_Z;
	}
	char out16;
	if(!p11) {
		out16 = state->p16 ? '1' : '0';
		pin16 = AS(state->p16);
	} else {
		out16 = 'Z';
		pin16 = sc_logic_Z;
	}
	char out15;
	if(!p11) {
		out15 = state->p15 ? '1' : '0';
		pin15 = AS(state->p15);
	} else {
		out15 = 'Z';
		pin15 = sc_logic_Z;
	}
	char out14;
	if(!p11) {
		out14 = state->p14 ? '1' : '0';
		pin14 = AS(state->p14);
	} else {
		out14 = 'Z';
		pin14 = sc_logic_Z;
	}
	char out13;
	if(!p11) {
		out13 = state->p13 ? '1' : '0';
		pin13 = AS(state->p13);
	} else {
		out13 = 'Z';
		pin13 = sc_logic_Z;
	}
	char out12;
	if(!p11) {
		out12 = state->p12 ? '1' : '0';
		pin12 = AS(state->p12);
	} else {
		out12 = 'Z';
		pin12 = sc_logic_Z;
	}

	if (dotrace) {
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
		    <<out12
		    <<out13
		    <<out14
		    <<out15
		    <<out16
		    <<out17
		    <<out18
		    <<out19
		);
	}
}
