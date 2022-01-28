// Generated from ../_Firmware/TAGAGAL-01.BIN by gal16v8.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "TAGAPAL.hh"

struct scm_tagapal_state {
	struct ctx ctx;
	bool p16;
	bool p17;
	bool p18;
	bool p19;
};

void
SCM_TAGAPAL :: loadit(const char *arg)
{
	state = (struct scm_tagapal_state *)
	    CTX_Get("tagapal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_TAGAPAL :: doit(void)
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
	bool p10 = IS_H(pin10);
	bool p11 = IS_H(pin11);
	bool p13 = IS_H(pin13);
	bool p14 = IS_H(pin14);
	bool p15 = IS_H(pin15);
	bool p16 = state->p16;
	bool p21 = IS_H(pin21);
	bool p22 = IS_H(pin22);
	bool p23 = IS_H(pin23);
	bool dotrace = false;
	dotrace = true;
	dotrace = true;
	dotrace = true;
	dotrace = true;
	dotrace = true;
	if (pin1.posedge()) {
		dotrace = true;
		state->p19 =
		    !(((!p06) && p11 && p22) ||
		    ((!p04) && (!p11) && p03 && p06 && p21) ||
		    (p06 && p11 && p23) ||
		    ((!p02) && (!p03) && (!p11) && p06 && p21) ||
		    ((!p08) && (!p11) && p06) ||
		    ((!p11) && p03 && p04 && p08 && p09) ||
		    ((!p11) && p02 && p04 && p08 && p09) ||
		    ((!p03) && (!p11) && p02 && p08 && p09) ||
		    ((!p02) && (!p03) && (!p04) && (!p05) && (!p11) && (!p21) && p09));
		state->p18 =
		    !(((!p06) && p11 && p14) ||
		    ((!p04) && (!p06) && (!p11) && p03 && p21) ||
		    (p06 && p11 && p15) ||
		    ((!p02) && (!p03) && (!p06) && (!p11) && p21) ||
		    ((!p06) && (!p08) && (!p11)) ||
		    ((!p09) && (!p11) && p03 && p04 && p08 && p10) ||
		    ((!p09) && (!p11) && p02 && p04 && p08 && p10) ||
		    ((!p03) && (!p09) && (!p11) && p02 && p08 && p10) ||
		    ((!p10) && (!p11) && p03 && p04 && p08 && p09) ||
		    ((!p10) && (!p11) && p02 && p04 && p08 && p09) ||
		    ((!p03) && (!p10) && (!p11) && p02 && p08 && p09) ||
		    ((!p02) && (!p03) && (!p04) && (!p05) && (!p09) && (!p11) && (!p21) && p10) ||
		    ((!p02) && (!p03) && (!p04) && (!p05) && (!p10) && (!p11) && (!p21) && p09));
		state->p17 =
		    !(((!p04) && p02 && p03 && p08) ||
		    ((!p02) && (!p04) && (!p05) && p03 && p08) ||
		    ((!p02) && (!p03) && p05 && p08) ||
		    ((!p02) && (!p03) && (!p05) && p04 && p08));
		state->p16 =
		    !(((!p04) && (!p06) && (!p07) && p02 && p03 && p08) ||
		    ((!p02) && (!p03) && (!p05) && (!p06) && (!p07) && p04 && p08) ||
		    ((!p13) && (!p16) && p06));
	}

	char out19 = state->p19 ? '1' : '0';
	pin19 = AS(state->p19);
	char out18 = state->p18 ? '1' : '0';
	pin18 = AS(state->p18);
	char out17 = state->p17 ? '1' : '0';
	pin17 = AS(state->p17);
	char out16 = state->p16 ? '1' : '0';
	pin16 = AS(state->p16);

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
		    <<pin10
		    <<pin11
		    <<pin13
		    <<pin14
		    <<pin15
		    <<pin21
		    <<pin22
		    <<pin23
		    << " | "
		    <<out16
		    <<out17
		    <<out18
		    <<out19
		);
	}
}
