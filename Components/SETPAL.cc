// Generated from ../_Firmware/SETGAL-01.BIN by gal_to_systemc.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "SETPAL.hh"

struct scm_setpal_state {
	struct ctx ctx;
	bool p18;
	bool p19;
	bool p20;
	bool p21;
	bool p22;
	bool p23;
};

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

	bool p06 = IS_H(pin6);
	bool p08 = IS_H(pin8);
	bool p09 = IS_H(pin9);
	bool p10 = IS_H(pin10);
	bool p11 = IS_H(pin11);
	bool p13 = IS_H(pin13);
	bool p18 = state->p18;
	bool p19 = state->p19;
	bool p20 = state->p20;
	bool p21 = state->p21;
	bool p22 = state->p22;
	bool p23 = state->p23;
	bool dotrace = false;
	dotrace = true;
	bool p17 =
	    !((p23) ||
	    (p22) ||
	    ((!p08)));
	dotrace = true;
	bool p16 =
	    !((p21) ||
	    (p20) ||
	    ((!p10)));
	if (pin1.posedge()) {
		dotrace = true;
		state->p23 =
		    ((!p06) && (!p08) && p10 && p11) ||
		    (p23 && p06);
		state->p22 =
		    ((!p06) && p08 && (!p09) && p10 && p11) ||
		    (p22 && p06);
		state->p21 =
		    ((!p06) && (!p10)) ||
		    (p21 && p06);
		state->p20 =
		    ((!p06) && p10 && (!p11)) ||
		    (p20 && p06);
		state->p19 =
		    (p20 && p06 && p13) ||
		    (p21 && p06 && p13) ||
		    (p23 && p06 && p10 && p11 && p13) ||
		    (p22 && p06 && p10 && p11 && p13) ||
		    (p19 && (!p06));
		state->p18 =
		    (p21 && p06 && p13) ||
		    ((!p20) && p06 && (!p10) && p13) ||
		    (p23 && p06 && p10 && p11 && p13) ||
		    ((!p22) && (!p20) && p06 && (!p08) && p11 && p13) ||
		    ((!p06) && p18);
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
	char out17 = p17 ? '1' : '0';
	pin17 = AS(p17);
	char out16 = p16 ? '1' : '0';
	pin16 = AS(p16);

	if (dotrace) {
		TRACE(
		    <<pin6
		    <<pin8
		    <<pin9
		    <<pin10
		    <<pin11
		    <<pin13
		    << " | "
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
