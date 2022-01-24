// Generated from on ../_Firmware/TSCGAL-01.BIN
#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "TSCPAL.hh"

struct scm_tscpal_state {
	struct ctx ctx;
	bool p14;
	bool p15;
	bool p16;
	bool p17;
	bool p18;
	bool p20;
	bool p21;
	bool p22;
	bool p23;
};

void
SCM_TSCPAL :: loadit(const char *arg)
{
	state = (struct scm_tscpal_state *)
	    CTX_Get("tscpal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_TSCPAL :: doit(void)
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
	bool p16 = state->p16;
	bool p17 = state->p17;
	bool dotrace = false;
	if (pin1.posedge()) {
		dotrace = true;
		state->p23 =
		    !(((!p11) && p08) ||
		    ((!p06) && (!p11)) ||
		    ((!p11) && p02) ||
		    ((!p03) && (!p11)) ||
		    ((!p04) && (!p11)) ||
		    ((!p05) && (!p11)));
		state->p22 =
		    !((p08 && p11) ||
		    ((!p06) && p11) ||
		    (p02 && p11) ||
		    ((!p03) && p11) ||
		    ((!p04) && p11) ||
		    ((!p05) && p11));
		state->p21 =
		    ((!p02) && (!p06) && (!p07) && p03 && p04 && p05 && p08 && p09);
		state->p20 =
		    ((!p02) && (!p06) && (!p07) && p03 && p04 && p05 && p08 && p10);
		state->p18 =
		    !(((!p02) && (!p06) && (!p07) && p03 && p04 && p05 && p08));
		state->p17 =
		    !(((!p02) && (!p06) && (!p07) && (!p11) && p03 && p04 && p05 && p08) ||
		    ((!p04) && (!p06) && (!p07) && p02 && p03 && p08) ||
		    ((!p02) && (!p03) && (!p05) && (!p06) && (!p07) && p04 && p08) ||
		    ((!p04) && (!p08) && (!p13) && (!p17) && p02 && p03 && p06) ||
		    ((!p02) && (!p03) && (!p05) && (!p08) && (!p13) && (!p17) && p04 && p06));
		state->p16 =
		    !(((!p02) && (!p06) && (!p07) && p03 && p04 && p05 && p08 && p11) ||
		    ((!p04) && (!p06) && (!p07) && p02 && p03 && p08) ||
		    ((!p02) && (!p03) && (!p05) && (!p06) && (!p07) && p04 && p08) ||
		    ((!p04) && (!p08) && (!p13) && (!p16) && p02 && p03 && p06) ||
		    ((!p02) && (!p03) && (!p05) && (!p08) && (!p13) && (!p16) && p04 && p06));
		state->p15 =
		    ((!p02) && (!p08) && p03 && p04 && p05 && p06);
		state->p14 =
		    !(((!p06) && (!p07)));
	}

	char out23 = state->p23 ? '1' : '0';
	pin23 = AS(state->p23);
	char out22 = state->p22 ? '1' : '0';
	pin22 = AS(state->p22);
	char out21 = state->p21 ? '1' : '0';
	pin21 = AS(state->p21);
	char out20 = state->p20 ? '1' : '0';
	pin20 = AS(state->p20);
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
		    <<out20
		    <<out21
		    <<out22
		    <<out23
		);
	}
}
