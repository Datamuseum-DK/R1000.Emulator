// Generated from ../_Firmware/TSXXGAL-01.BIN by gal_to_systemc.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "TSXXPAL.hh"

struct scm_tsxxpal_state {
	struct ctx ctx;
	int job;
	int p17;
	int p18;
	int p20;
	int p21;
};
static const sc_logic outs[4] = {sc_logic_Z, sc_logic_Z, sc_logic_0, sc_logic_1};
static const char traces[5] = "ZZ01";

SCM_TSXXPAL :: SCM_TSXXPAL(sc_module_name nm, const char *arg)
{
	SC_METHOD(doit);
	sensitive << pin1.pos();

	state = (struct scm_tsxxpal_state *)
	    CTX_Get("tsxxpal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_TSXXPAL :: doit(void)
{

	state->ctx.activations++;

	assert(0 <= state->p17);
	assert(3 >= state->p17);
	assert(0 <= state->p18);
	assert(3 >= state->p18);
	assert(0 <= state->p20);
	assert(3 >= state->p20);
	assert(0 <= state->p21);
	assert(3 >= state->p21);
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
		    <<pin11
		    <<pin13
		    <<pin14
		    <<pin15
		    <<pin16
		    <<pin22
		    <<pin23
		    << " | "
		    <<traces[state->p17]
		    <<traces[state->p18]
		    <<traces[state->p20]
		    <<traces[state->p21]
		);
		pin17 = outs[state->p17];
		pin18 = outs[state->p18];
		pin20 = outs[state->p20];
		pin21 = outs[state->p21];
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
	bool p11 = IS_H(pin11);
	bool p13 = IS_H(pin13);
	bool p14 = IS_H(pin14);
	bool p15 = IS_H(pin15);
	bool p16 = IS_H(pin16);
	bool p22 = IS_H(pin22);
	bool p23 = IS_H(pin23);
	int out21 = state->p21 & 1;
	int out20 = state->p20 & 1;
	int out18 = state->p18 & 1;
	int out17 = state->p17 & 1;
	if (pin1.posedge()) {
		out21 =
		    ((!p23) && p22 && (!p04) && (!p05) && (!p06) && (!p07) && (!p16) && (!p15) && (!p14) && p11) ||
		    (p23 && (!p22) && (!p16) && (!p15) && p14 && p11 && (!p13)) ||
		    (p23 && (!p02) && (!p22) && (!p03) && (!p16) && (!p15) && (!p14) && p11) ||
		    ((!p09) && (!p11)) ||
		    (p22 && (!p16) && (!p09) && p15 && p11) ||
		    (p22 && (!p16) && (!p09) && (!p15) && p14 && p11) ||
		    ((!p23) && (!p22) && (!p16) && (!p09) && (!p15) && p14 && p11) ||
		    (p23 && (!p22) && (!p16) && (!p09) && p15 && (!p14) && p11) ||
		    (p16 && p11 && (!p13));
		out20 =
		    (p02 && (!p22) && (!p03) && (!p16) && p15 && p14 && p11) ||
		    ((!p23) && (!p22) && p03 && (!p16) && p15 && (!p14) && p11) ||
		    ((!p23) && p02 && (!p22) && (!p03) && (!p16) && p15 && (!p14) && p11) ||
		    (p23 && p22 && p03 && (!p16) && (!p15) && (!p14) && p11) ||
		    (p23 && p02 && p22 && (!p03) && (!p16) && (!p15) && (!p14) && p11) ||
		    ((!p23) && (!p02) && (!p22) && p03 && (!p16) && p15 && p14 && p11);
		out18 =
		    (p23 && (!p22) && (!p08) && (!p16) && p15 && p14);
		out17 =
		    ((!p04) && p05 && p06 && p07) ||
		    (p04 && (!p05) && p06 && p07) ||
		    (p04 && p05 && (!p06) && p07) ||
		    ((!p04) && (!p05) && (!p06) && p07) ||
		    (p04 && p05 && p06 && (!p07)) ||
		    ((!p04) && (!p05) && p06 && (!p07)) ||
		    ((!p04) && p05 && (!p06) && (!p07)) ||
		    (p04 && (!p05) && (!p06) && (!p07));
	}

	assert(0 <= out21 && out21 <= 1);
	out21 += 2;
	assert(0 <= out20 && out20 <= 1);
	out20 += 2;
	assert(0 <= out18 && out18 <= 1);
	out18 += 2;
	assert(0 <= out17 && out17 <= 1);
	out17 += 2;

	if (
	    (out21 != state->p21) ||
	    (out20 != state->p20) ||
	    (out18 != state->p18) ||
	    (out17 != state->p17)) {
		state->p21 = out21;
		state->p20 = out20;
		state->p18 = out18;
		state->p17 = out17;
		state->job = 1;
		next_trigger(5, SC_NS);
	}
}
