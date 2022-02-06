// Generated from ../_Firmware/DISTGAL-02.BIN by gal_to_systemc.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "DISTPAL.hh"

struct scm_distpal_state {
	struct ctx ctx;
	bool p16;
	bool p17;
	bool p18;
	bool p19;
};

SCM_DISTPAL :: SCM_DISTPAL(sc_module_name nm, const char *arg)
{
	SC_METHOD(doit);
	sensitive << pin1.pos();

	state = (struct scm_distpal_state *)
	    CTX_Get("distpal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_DISTPAL :: doit(void)
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
	bool p16 = state->p16;
	bool p17 = state->p17;
	bool p18 = state->p18;
	bool p19 = state->p19;
	bool dotrace = false;
	if (pin1.posedge()) {
		dotrace = true;
		state->p19 =
		    !(((!p08) && (!p09)) ||
		    ((!p05) && (!p07) && (!p08)) ||
		    ((!p19) && p06 && p07 && (!p08)) ||
		    ((!p19) && (!p18) && p07 && (!p08)) ||
		    ((!p19) && (!p17) && p07 && (!p08)) ||
		    ((!p19) && (!p16) && p07 && (!p08)) ||
		    (p19 && p18 && p17 && p16 && (!p06) && p07 && (!p08)));
		state->p18 =
		    !(((!p08) && (!p09)) ||
		    ((!p02) && (!p07) && (!p08)) ||
		    ((!p18) && p06 && p07 && (!p08)) ||
		    ((!p18) && (!p17) && p07 && (!p08)) ||
		    ((!p18) && (!p16) && p07 && (!p08)) ||
		    (p18 && p17 && p16 && (!p06) && p07 && (!p08)));
		state->p17 =
		    !(((!p08) && (!p09)) ||
		    ((!p03) && (!p07) && (!p08)) ||
		    ((!p17) && p06 && p07 && (!p08)) ||
		    ((!p17) && (!p16) && p07 && (!p08)) ||
		    (p17 && p16 && (!p06) && p07 && (!p08)));
		state->p16 =
		    !(((!p08) && (!p09)) ||
		    ((!p04) && (!p07) && (!p08)) ||
		    (p16 && (!p06) && p07 && (!p08)) ||
		    ((!p16) && p06 && p07 && (!p08)));
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
		    <<pin8
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
