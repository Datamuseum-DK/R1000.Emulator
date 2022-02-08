// Generated from ../_Firmware/DIBRGAL-02.BIN by gal_to_systemc.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "DIBRPAL.hh"

struct scm_dibrpal_state {
	struct ctx ctx;
	int job;
	int p12;
	int p17;
	int p18;
	int p19;
};
static const sc_logic outs[4] = {sc_logic_Z, sc_logic_Z, sc_logic_0, sc_logic_1};
static const char traces[5] = "ZZ01";

SCM_DIBRPAL :: SCM_DIBRPAL(sc_module_name nm, const char *arg)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin11 << pin2 << pin3 << pin4 << pin5 << pin6 << pin7 << pin8 << pin9;

	state = (struct scm_dibrpal_state *)
	    CTX_Get("dibrpal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_DIBRPAL :: doit(void)
{

	state->ctx.activations++;

	assert(0 <= state->p12);
	assert(3 >= state->p12);
	assert(0 <= state->p17);
	assert(3 >= state->p17);
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
		    <<traces[state->p17]
		    <<traces[state->p18]
		    <<traces[state->p19]
		);
		pin12 = outs[state->p12];
		pin17 = outs[state->p17];
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
	int out17;
	int out12;
	out19 =
	    !(((!p02) && (!p03) && (!p04) && (!p05)) ||
	    (p02 && (!p01)) ||
	    ((!p01) && p03) ||
	    ((!p01) && p04) ||
	    ((!p01) && (!p05)));
	out18 =
	    !(((!p04) && (!p05) && (!p09)) ||
	    ((!p04) && p05 && p09) ||
	    (p04 && (!p05) && p08) ||
	    (p04 && p05 && (!p08)));
	out17 =
	    !((p03 && p04 && p05 && p06) ||
	    (p03 && p04 && (!p05) && (!p06)) ||
	    (p03 && (!p04) && p05 && (!p07)) ||
	    (p03 && (!p04) && (!p05) && p07) ||
	    ((!p03) && p04 && p05 && (!p11)) ||
	    ((!p03) && p04 && (!p05) && p11) ||
	    ((!p03) && (!p04)));
	out12 =
	    !((p02 && p03 && (!p06)) ||
	    (p02 && (!p03)) ||
	    ((!p02) && p03));

	assert(0 <= out19 && out19 <= 1);
	out19 += 2;
	assert(0 <= out18 && out18 <= 1);
	if(p02)
		out18 += 2;
	assert(0 <= out17 && out17 <= 1);
	if((!p02))
		out17 += 2;
	assert(0 <= out12 && out12 <= 1);
	out12 += 2;

	if (
	    (out19 != state->p19) ||
	    (out18 != state->p18) ||
	    (out17 != state->p17) ||
	    (out12 != state->p12)) {
		state->p19 = out19;
		state->p18 = out18;
		state->p17 = out17;
		state->p12 = out12;
		state->job = 1;
		next_trigger(5, SC_NS);
	}
}
