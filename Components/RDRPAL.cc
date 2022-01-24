// Generated from on ../_Firmware/RDRGAL-02.BIN
#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "RDRPAL.hh"

struct scm_rdrpal_state {
	struct ctx ctx;
	bool p16;
	bool p17;
	bool p18;
	bool p19;
};

void
SCM_RDRPAL :: loadit(const char *arg)
{
	state = (struct scm_rdrpal_state *)
	    CTX_Get("rdrpal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_RDRPAL :: doit(void)
{

	state->ctx.activations++;

	bool p02 = IS_H(pin2);
	bool p03 = IS_H(pin3);
	bool p04 = IS_H(pin4);
	bool p05 = IS_H(pin5);
	bool p06 = IS_H(pin6);
	bool p07 = IS_H(pin7);
	bool p09 = IS_H(pin9);
	bool p11 = IS_H(pin11);
	bool dotrace = false;
	if (pin1.posedge()) {
		dotrace = true;
		state->p19 =
		    !(((!p04) && (!p05) && (!p07) && p02 && p03 && p06) ||
		    ((!p05) && (!p07) && p02 && p03 && p04 && p06) ||
		    ((!p02) && (!p04) && p03 && p05 && p06));
		state->p18 =
		    !(((!p05) && (!p07) && p02 && p03 && p04 && p06) ||
		    ((!p02) && (!p05) && (!p07) && p03 && p04 && p06 && p09) ||
		    ((!p04) && (!p05) && (!p07) && p02 && p03 && p06) ||
		    ((!p02) && (!p04) && p03 && p05 && p06));
		state->p17 =
		    !(((!p05) && (!p07) && p02 && p03 && p04 && p06) ||
		    ((!p02) && (!p05) && (!p07) && (!p09) && p03 && p04 && p06) ||
		    ((!p04) && (!p05) && (!p07) && p02 && p03 && p06) ||
		    ((!p02) && (!p04) && p03 && p05 && p06));
		state->p16 =
		    !(((!p02) && (!p05) && (!p07) && p03 && p04 && p06) ||
		    ((!p02) && (!p04) && p03 && p05 && p06));
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

	if (dotrace) {
		TRACE(
		    <<pin2
		    <<pin3
		    <<pin4
		    <<pin5
		    <<pin6
		    <<pin7
		    <<pin9
		    <<pin11
		    << " | "
		    <<out16
		    <<out17
		    <<out18
		    <<out19
		);
	}
}
