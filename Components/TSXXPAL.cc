// Generated from ../_Firmware/TSXXGAL-01.BIN by gal16v8.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "TSXXPAL.hh"

struct scm_tsxxpal_state {
	struct ctx ctx;
	bool p17;
	bool p18;
	bool p20;
	bool p21;
};

void
SCM_TSXXPAL :: loadit(const char *arg)
{
	state = (struct scm_tsxxpal_state *)
	    CTX_Get("tsxxpal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_TSXXPAL :: doit(void)
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
	bool p13 = IS_H(pin13);
	bool p14 = IS_H(pin14);
	bool p15 = IS_H(pin15);
	bool p16 = IS_H(pin16);
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
		state->p21 =
		    ((!p23) && p22 && (!p04) && (!p05) && (!p06) && (!p07) && (!p16) && (!p15) && (!p14) && p11) ||
		    (p23 && (!p22) && (!p16) && (!p15) && p14 && p11 && (!p13)) ||
		    (p23 && (!p02) && (!p22) && (!p03) && (!p16) && (!p15) && (!p14) && p11) ||
		    ((!p09) && (!p11)) ||
		    (p22 && (!p16) && (!p09) && p15 && p11) ||
		    (p22 && (!p16) && (!p09) && (!p15) && p14 && p11) ||
		    ((!p23) && (!p22) && (!p16) && (!p09) && (!p15) && p14 && p11) ||
		    (p23 && (!p22) && (!p16) && (!p09) && p15 && (!p14) && p11) ||
		    (p16 && p11 && (!p13));
		state->p20 =
		    (p02 && (!p22) && (!p03) && (!p16) && p15 && p14 && p11) ||
		    ((!p23) && (!p22) && p03 && (!p16) && p15 && (!p14) && p11) ||
		    ((!p23) && p02 && (!p22) && (!p03) && (!p16) && p15 && (!p14) && p11) ||
		    (p23 && p22 && p03 && (!p16) && (!p15) && (!p14) && p11) ||
		    (p23 && p02 && p22 && (!p03) && (!p16) && (!p15) && (!p14) && p11) ||
		    ((!p23) && (!p02) && (!p22) && p03 && (!p16) && p15 && p14 && p11);
		state->p18 =
		    (p23 && (!p22) && (!p08) && (!p16) && p15 && p14);
		state->p17 =
		    ((!p04) && p05 && p06 && p07) ||
		    (p04 && (!p05) && p06 && p07) ||
		    (p04 && p05 && (!p06) && p07) ||
		    ((!p04) && (!p05) && (!p06) && p07) ||
		    (p04 && p05 && p06 && (!p07)) ||
		    ((!p04) && (!p05) && p06 && (!p07)) ||
		    ((!p04) && p05 && (!p06) && (!p07)) ||
		    (p04 && (!p05) && (!p06) && (!p07));
	}

	char out21 = state->p21 ? '1' : '0';
	pin21 = AS(state->p21);
	char out20 = state->p20 ? '1' : '0';
	pin20 = AS(state->p20);
	char out18 = state->p18 ? '1' : '0';
	pin18 = AS(state->p18);
	char out17 = state->p17 ? '1' : '0';
	pin17 = AS(state->p17);

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
		    <<pin13
		    <<pin14
		    <<pin15
		    <<pin16
		    <<pin22
		    <<pin23
		    << " | "
		    <<out17
		    <<out18
		    <<out20
		    <<out21
		);
	}
}
