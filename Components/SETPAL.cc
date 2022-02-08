// Generated from ../_Firmware/SETGAL-01.BIN by gal_to_systemc.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "SETPAL.hh"

struct scm_setpal_state {
	struct ctx ctx;
	int job;
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

SCM_SETPAL :: SCM_SETPAL(sc_module_name nm, const char *arg)
{
	SC_METHOD(doit);
	sensitive << pin1.pos() << pin10 << pin8;

	state = (struct scm_setpal_state *)
	    CTX_Get("setpal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_SETPAL :: doit(void)
{

	state->ctx.activations++;

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
		    <<pin6
		    <<pin8
		    <<pin9
		    <<pin10
		    <<pin11
		    <<pin13
		    << " | "
		    <<traces[state->p16]
		    <<traces[state->p17]
		    <<traces[state->p18]
		    <<traces[state->p19]
		    <<traces[state->p20]
		    <<traces[state->p21]
		    <<traces[state->p22]
		    <<traces[state->p23]
		);
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
	bool p06 = IS_H(pin6);
	bool p08 = IS_H(pin8);
	bool p09 = IS_H(pin9);
	bool p10 = IS_H(pin10);
	bool p11 = IS_H(pin11);
	bool p13 = IS_H(pin13);
	bool p18 = state->p18 % 2;
	bool p19 = state->p19 % 2;
	bool p20 = state->p20 % 2;
	bool p21 = state->p21 % 2;
	bool p22 = state->p22 % 2;
	bool p23 = state->p23 % 2;
	int out23 = state->p23 & 1;
	int out22 = state->p22 & 1;
	int out21 = state->p21 & 1;
	int out20 = state->p20 & 1;
	int out19 = state->p19 & 1;
	int out18 = state->p18 & 1;
	int out17;
	int out16;
	out17 =
	    !((p23) ||
	    (p22) ||
	    ((!p08)));
	out16 =
	    !((p21) ||
	    (p20) ||
	    ((!p10)));
	if (pin1.posedge()) {
		out23 =
		    ((!p06) && (!p08) && p10 && p11) ||
		    (p23 && p06);
		out22 =
		    ((!p06) && p08 && (!p09) && p10 && p11) ||
		    (p22 && p06);
		out21 =
		    ((!p06) && (!p10)) ||
		    (p21 && p06);
		out20 =
		    ((!p06) && p10 && (!p11)) ||
		    (p20 && p06);
		out19 =
		    (p20 && p06 && p13) ||
		    (p21 && p06 && p13) ||
		    (p23 && p06 && p10 && p11 && p13) ||
		    (p22 && p06 && p10 && p11 && p13) ||
		    (p19 && (!p06));
		out18 =
		    (p21 && p06 && p13) ||
		    ((!p20) && p06 && (!p10) && p13) ||
		    (p23 && p06 && p10 && p11 && p13) ||
		    ((!p22) && (!p20) && p06 && (!p08) && p11 && p13) ||
		    ((!p06) && p18);
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
	assert(0 <= out17 && out17 <= 1);
	out17 += 2;
	assert(0 <= out16 && out16 <= 1);
	out16 += 2;

	if (
	    (out23 != state->p23) ||
	    (out22 != state->p22) ||
	    (out21 != state->p21) ||
	    (out20 != state->p20) ||
	    (out19 != state->p19) ||
	    (out18 != state->p18) ||
	    (out17 != state->p17) ||
	    (out16 != state->p16)) {
		state->p23 = out23;
		state->p22 = out22;
		state->p21 = out21;
		state->p20 = out20;
		state->p19 = out19;
		state->p18 = out18;
		state->p17 = out17;
		state->p16 = out16;
		state->job = 1;
		next_trigger(5, SC_NS);
	}
}
