// Generated from ../_Firmware/DRADGAL-01.BIN by gal_to_systemc.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "DRADPAL.hh"

struct scm_dradpal_state {
	struct ctx ctx;
	int job;
	int p18;
	int p19;
	int p20;
	int p21;
	int p22;
	int p23;
};
static const sc_logic outs[4] = {sc_logic_Z, sc_logic_Z, sc_logic_0, sc_logic_1};
static const char traces[5] = "ZZ01";

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

	assert(0 <= state->p18);
	assert(3 >= state->p18);
	assert(0 <= state->p19);
	assert(3 >= state->p19);
	assert(0 <= state->p20);
	assert(3 >= state->p20);
	assert(0 <= state->p21);
	assert(3 >= state->p21);
	assert(0 <= state->p22);
	assert(3 >= state->p22);
	assert(0 <= state->p23);
	assert(3 >= state->p23);
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
		    <<pin10
		    <<pin11
		    <<pin13
		    <<pin14
		    <<pin15
		    <<pin16
		    <<pin17
		    << " | "
		    <<traces[state->p18]
		    <<traces[state->p19]
		    <<traces[state->p20]
		    <<traces[state->p21]
		    <<traces[state->p22]
		    <<traces[state->p23]
		);
		pin18 = outs[state->p18];
		pin19 = outs[state->p19];
		pin20 = outs[state->p20];
		pin21 = outs[state->p21];
		pin22 = outs[state->p22];
		pin23 = outs[state->p23];
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
	bool p10 = IS_H(pin10);
	bool p11 = IS_H(pin11);
	bool p13 = IS_H(pin13);
	bool p14 = IS_H(pin14);
	bool p15 = IS_H(pin15);
	bool p16 = IS_H(pin16);
	bool p17 = IS_H(pin17);
	bool p19 = state->p19 % 2;
	bool p21 = state->p21 % 2;
	bool p22 = state->p22 % 2;
	bool p23 = state->p23 % 2;
	int out23 = state->p23 & 1;
	int out22 = state->p22 & 1;
	int out21 = state->p21 & 1;
	int out20 = state->p20 & 1;
	int out19 = state->p19 & 1;
	int out18 = state->p18 & 1;
	if (pin1.posedge()) {
		out23 =
		    (p03 && (!p04) && (!p05) && (!p06) && p07 && p17 && p16 && p15 && p14) ||
		    ((!p02) && (!p03) && (!p05) && (!p06) && p07 && p17 && p16 && p15 && p14);
		out22 =
		    !(((!p07)));
		out21 =
		    (p05 && p11) ||
		    (p21 && (!p05) && (!p07)) ||
		    ((!p05) && p06 && p07 && p09) ||
		    ((!p05) && (!p06) && p07 && p17 && p16 && p15 && p14);
		out20 =
		    (p05 && p11) ||
		    (p21 && (!p05) && (!p07)) ||
		    ((!p05) && p06 && p07 && p09) ||
		    ((!p05) && (!p06) && p07 && p17 && p16 && p15 && p14);
		out19 =
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
		out18 =
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

	assert(0 <= out23 && out23 <= 1);
	out23 += 2;
	assert(0 <= out22 && out22 <= 1);
	out22 += 2;
	assert(0 <= out21 && out21 <= 1);
	out21 += 2;
	assert(0 <= out20 && out20 <= 1);
	out20 += 2;
	assert(0 <= out19 && out19 <= 1);
	out19 += 2;
	assert(0 <= out18 && out18 <= 1);
	out18 += 2;

	if (
	    (out23 != state->p23) ||
	    (out22 != state->p22) ||
	    (out21 != state->p21) ||
	    (out20 != state->p20) ||
	    (out19 != state->p19) ||
	    (out18 != state->p18)) {
		state->p23 = out23;
		state->p22 = out22;
		state->p21 = out21;
		state->p20 = out20;
		state->p19 = out19;
		state->p18 = out18;
		state->job = 1;
		next_trigger(5, SC_NS);
	}
}
