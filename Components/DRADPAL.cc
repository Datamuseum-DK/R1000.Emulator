// Generated from ../_Firmware/DRADGAL-01.BIN by gal_to_systemc.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "DRADPAL.hh"

struct scm_dradpal_state {
	struct ctx ctx;
	bool p18;
	bool p19;
	bool p20;
	bool p21;
	bool p22;
	bool p23;
};

SCM_DRADPAL :: SCM_DRADPAL(sc_module_name nm, const char *arg)
{
	SC_METHOD(doit);
	sensitive << pin1.pos();

	state = (struct scm_dradpal_state *)
	    CTX_Get("dradpal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_DRADPAL :: doit(void)
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
	bool p16 = IS_H(pin16);
	bool p17 = IS_H(pin17);
	bool p19 = state->p19;
	bool p21 = state->p21;
	bool p22 = state->p22;
	bool p23 = state->p23;
	bool dotrace = false;
	dotrace = true;
	dotrace = true;
	dotrace = true;
	dotrace = true;
	if (pin1.posedge()) {
		dotrace = true;
		state->p23 =
		    (p03 && (!p04) && (!p05) && (!p06) && p07 && p17 && p16 && p15 && p14) ||
		    ((!p02) && (!p03) && (!p05) && (!p06) && p07 && p17 && p16 && p15 && p14);
		state->p22 =
		    !(((!p07)));
		state->p21 =
		    (p05 && p11) ||
		    (p21 && (!p05) && (!p07)) ||
		    ((!p05) && p06 && p07 && p09) ||
		    ((!p05) && (!p06) && p07 && p17 && p16 && p15 && p14);
		state->p20 =
		    (p05 && p11) ||
		    (p21 && (!p05) && (!p07)) ||
		    ((!p05) && p06 && p07 && p09) ||
		    ((!p05) && (!p06) && p07 && p17 && p16 && p15 && p14);
		state->p19 =
		    (p05 && p13) ||
		    ((!p23) && (!p05) && p19 && (!p07)) ||
		    ((!p22) && (!p05) && p19 && (!p07)) ||
		    ((!p05) && p06 && p07 && p10) ||
		    (p23 && p22 && (!p05) && (!p07) && p17 && p15) ||
		    ((!p05) && (!p06) && p07 && p17 && (!p16) && p15) ||
		    ((!p05) && (!p06) && p07 && p17 && p15 && (!p14)) ||
		    ((!p05) && (!p06) && p07 && p17 && p08 && p15) ||
		    (p03 && (!p04) && (!p05) && (!p06) && p07 && p17 && p15) ||
		    ((!p02) && (!p03) && (!p05) && (!p06) && p07 && p17 && p15);
		state->p18 =
		    (p05 && p13) ||
		    ((!p23) && (!p05) && p19 && (!p07)) ||
		    ((!p22) && (!p05) && p19 && (!p07)) ||
		    ((!p05) && p06 && p07 && p10) ||
		    (p23 && p22 && (!p05) && (!p07) && p17 && p15) ||
		    ((!p05) && (!p06) && p07 && p17 && (!p16) && p15) ||
		    ((!p05) && (!p06) && p07 && p17 && p15 && (!p14)) ||
		    ((!p05) && (!p06) && p07 && p17 && p08 && p15) ||
		    (p03 && (!p04) && (!p05) && (!p06) && p07 && p17 && p15) ||
		    ((!p02) && (!p03) && (!p05) && (!p06) && p07 && p17 && p15);
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
		    <<pin16
		    <<pin17
		    << " | "
		    <<out18
		    <<out19
		    <<out20
		    <<out21
		    <<out22
		    <<out23
		);
	}
}
