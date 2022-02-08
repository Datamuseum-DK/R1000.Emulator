// Generated from ../_Firmware/TRACGAL-01.BIN by gal_to_systemc.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "TRACPAL.hh"

struct scm_tracpal_state {
	struct ctx ctx;
	int job;
	int p14;
	int p15;
	int p16;
	int p17;
	int p18;
	int p19;
	int p20;
	int p21;
	int p22;
	int p23;
};
static const sc_logic outs[4] = {sc_logic_Z, sc_logic_Z, sc_logic_0, sc_logic_1};
static const char traces[5] = "ZZ01";

SCM_TRACPAL :: SCM_TRACPAL(sc_module_name nm, const char *arg)
{
	SC_METHOD(doit);
	sensitive << pin2 << pin3 << pin4 << pin5 << pin6 << pin7;

	state = (struct scm_tracpal_state *)
	    CTX_Get("tracpal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_TRACPAL :: doit(void)
{

	state->ctx.activations++;

	assert(0 <= state->p14);
	assert(3 >= state->p14);
	assert(0 <= state->p15);
	assert(3 >= state->p15);
	assert(0 <= state->p16);
	assert(3 >= state->p16);
	assert(0 <= state->p17);
	assert(3 >= state->p17);
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
		    << " | "
		    <<traces[state->p14]
		    <<traces[state->p15]
		    <<traces[state->p16]
		    <<traces[state->p17]
		    <<traces[state->p18]
		    <<traces[state->p19]
		    <<traces[state->p20]
		    <<traces[state->p21]
		    <<traces[state->p22]
		    <<traces[state->p23]
		);
		pin14 = outs[state->p14];
		pin15 = outs[state->p15];
		pin16 = outs[state->p16];
		pin17 = outs[state->p17];
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
	int out23;
	int out22;
	int out21;
	int out20;
	int out19;
	int out18;
	int out17;
	int out16;
	int out15;
	int out14;
	out23 =
	    !(((!p03)) ||
	    (p04 && (!p05) && p06) ||
	    ((!p04) && (!p05) && (!p06)));
	out22 =
	    ((!p02) && (!p03) && p04) ||
	    (p03 && p04 && (!p05) && p06);
	out21 =
	    !(((!p07)) ||
	    (p03 && (!p04) && p05));
	out20 =
	    !(((!p07)) ||
	    ((!p03)) ||
	    (p04 && p05 && p06) ||
	    (p04 && (!p05) && (!p06)));
	out19 =
	    !((p02 && p03 && p04 && p06) ||
	    (p03 && p04 && p05 && p06));
	out18 =
	    (p03 && (!p04) && p05 && p06);
	out17 =
	    !(((!p07)) ||
	    (p03 && (!p04) && (!p05) && (!p06)) ||
	    ((!p02) && p03 && p04 && (!p05) && p06));
	out16 =
	    ((!p03) && p05) ||
	    (p02 && p03 && p04 && (!p05) && p06) ||
	    (p02 && p03 && (!p04) && (!p05) && (!p06));
	out15 =
	    ((!p03) && p05 && (!p06)) ||
	    ((!p03) && (!p05) && p06) ||
	    (p02 && p03 && (!p04) && (!p05) && (!p06));
	out14 =
	    (p03 && (!p04) && (!p05) && (!p06));

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
	assert(0 <= out17 && out17 <= 1);
	out17 += 2;
	assert(0 <= out16 && out16 <= 1);
	out16 += 2;
	assert(0 <= out15 && out15 <= 1);
	out15 += 2;
	assert(0 <= out14 && out14 <= 1);
	out14 += 2;

	if (
	    (out23 != state->p23) ||
	    (out22 != state->p22) ||
	    (out21 != state->p21) ||
	    (out20 != state->p20) ||
	    (out19 != state->p19) ||
	    (out18 != state->p18) ||
	    (out17 != state->p17) ||
	    (out16 != state->p16) ||
	    (out15 != state->p15) ||
	    (out14 != state->p14)) {
		state->p23 = out23;
		state->p22 = out22;
		state->p21 = out21;
		state->p20 = out20;
		state->p19 = out19;
		state->p18 = out18;
		state->p17 = out17;
		state->p16 = out16;
		state->p15 = out15;
		state->p14 = out14;
		state->job = 1;
		next_trigger(5, SC_NS);
	}
}
