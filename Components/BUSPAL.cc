// Generated from ../_Firmware/BUSGAL-01.BIN by gal16v8.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "BUSPAL.hh"

struct scm_buspal_state {
	struct ctx ctx;
	bool p14;
	bool p15;
	bool p16;
	bool p17;
	bool p18;
	bool p19;
	bool p20;
	bool p21;
	bool p22;
	bool p23;
};

void
SCM_BUSPAL :: loadit(const char *arg)
{
	state = (struct scm_buspal_state *)
	    CTX_Get("buspal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_BUSPAL :: doit(void)
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
	bool dotrace = false;
	if (pin1.posedge()) {
		dotrace = true;
		state->p23 =
		    !(((!p03) && (!p05) && (!p06) && (!p07) && (!p08) && (!p09) && (!p11) && p02 && p04 && p10) ||
		    ((!p02) && (!p06) && (!p07) && (!p08) && (!p09) && p03 && p04 && p05 && p10) ||
		    ((!p07) && (!p08) && (!p10)) ||
		    ((!p07) && p08));
		state->p22 =
		    !(((!p03) && (!p05) && (!p06) && (!p08) && (!p10) && (!p13) && p02 && p04 && p07 && p09) ||
		    ((!p02) && (!p06) && (!p07) && (!p08) && (!p09) && p03 && p04 && p05 && p10) ||
		    ((!p08) && (!p09) && (!p10) && p07) ||
		    ((!p07) && p08) ||
		    ((!p07) && (!p08) && (!p10) && p09));
		state->p21 =
		    ((!p07) && (!p08) && p10) ||
		    ((!p07) && (!p09) && (!p10) && p08);
		state->p20 =
		    ((!p07) && (!p08) && (!p09) && p10) ||
		    ((!p07) && p08 && p09);
		state->p19 =
		    ((!p03) && (!p06) && (!p07) && (!p08) && (!p09) && (!p11) && p02 && p04 && p10) ||
		    ((!p06) && (!p07) && (!p08) && (!p09) && p02 && p03 && p05 && p10) ||
		    ((!p02) && (!p06) && (!p07) && (!p08) && (!p09) && p03 && p04 && p05 && p10) ||
		    ((!p07) && (!p08) && (!p09) && (!p10)) ||
		    ((!p07) && p08) ||
		    ((!p07) && (!p08) && p09);
		state->p18 =
		    ((!p03) && (!p05) && (!p06) && (!p08) && (!p10) && (!p13) && p02 && p04 && p07 && p09) ||
		    ((!p03) && (!p06) && (!p07) && (!p08) && (!p09) && (!p13) && p02 && p04 && p05 && p10) ||
		    ((!p06) && (!p07) && (!p08) && (!p09) && p02 && p03 && p05 && p10) ||
		    ((!p02) && (!p06) && (!p07) && (!p08) && (!p09) && p03 && p04 && p05 && p10) ||
		    ((!p08) && (!p09) && (!p10) && p07) ||
		    ((!p07) && p08) ||
		    ((!p07) && (!p08) && p09 && p10);
		state->p17 =
		    !(((!p03) && (!p05) && (!p06) && (!p07) && (!p08) && (!p09) && p02 && p04 && p10 && p13) ||
		    ((!p03) && (!p05) && (!p06) && (!p08) && (!p10) && p02 && p04 && p07 && p09 && p13) ||
		    ((!p02) && (!p06) && (!p07) && (!p08) && (!p09) && p03 && p04 && p10 && p13) ||
		    ((!p08) && (!p09) && (!p10) && p07) ||
		    ((!p07) && p08) ||
		    ((!p07) && (!p08) && (!p10) && p09));
		state->p16 =
		    !(((!p03) && (!p05) && (!p06) && (!p07) && (!p08) && (!p09) && (!p13) && p02 && p04 && p10) ||
		    ((!p03) && (!p05) && (!p06) && (!p08) && (!p10) && (!p13) && p02 && p04 && p07 && p09) ||
		    ((!p02) && (!p06) && (!p07) && (!p08) && (!p09) && (!p13) && p03 && p04 && p10) ||
		    ((!p07) && (!p08) && (!p10)) ||
		    ((!p07) && p08));
		state->p15 =
		    !(((!p07) && (!p08) && (!p10) && p09));
		state->p14 =
		    ((!p07) && (!p08) && p10) ||
		    ((!p07) && (!p09) && p08 && p10);
	}

	char out23 = state->p23 ? '1' : '0';
	pin23 = AS(state->p23);
	char out22 = state->p22 ? '1' : '0';
	pin22 = AS(state->p22);
	char out21 = state->p21 ? '1' : '0';
	pin21 = AS(state->p21);
	char out20 = state->p20 ? '1' : '0';
	pin20 = AS(state->p20);
	char out19 = state->p19 ? '1' : '0';
	pin19 = AS(state->p19);
	char out18 = state->p18 ? '1' : '0';
	pin18 = AS(state->p18);
	char out17 = state->p17 ? '1' : '0';
	pin17 = AS(state->p17);
	char out16 = state->p16 ? '1' : '0';
	pin16 = AS(state->p16);
	char out15 = state->p15 ? '1' : '0';
	pin15 = AS(state->p15);
	char out14 = state->p14 ? '1' : '0';
	pin14 = AS(state->p14);

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
		    << " | "
		    <<out14
		    <<out15
		    <<out16
		    <<out17
		    <<out18
		    <<out19
		    <<out20
		    <<out21
		    <<out22
		    <<out23
		);
	}
}
