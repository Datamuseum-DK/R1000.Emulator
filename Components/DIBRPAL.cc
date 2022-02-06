// Generated from ../_Firmware/DIBRGAL-02.BIN by gal16v8.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "DIBRPAL.hh"

struct scm_dibrpal_state {
	struct ctx ctx;
};

void
SCM_DIBRPAL :: loadit(const char *arg)
{
	state = (struct scm_dibrpal_state *)
	    CTX_Get("dibrpal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_DIBRPAL :: doit(void)
{

	state->ctx.activations++;

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
	bool dotrace = false;
	dotrace = true;
	bool p19 =
	    !(((!p02) && (!p03) && (!p04) && (!p05)) ||
	    (p02 && (!p01)) ||
	    ((!p01) && p03) ||
	    ((!p01) && p04) ||
	    ((!p01) && (!p05)));
	dotrace = true;
	bool p18 =
	    !(((!p04) && (!p05) && (!p09)) ||
	    ((!p04) && p05 && p09) ||
	    (p04 && (!p05) && p08) ||
	    (p04 && p05 && (!p08)));
	dotrace = true;
	bool p17 =
	    !((p03 && p04 && p05 && p06) ||
	    (p03 && p04 && (!p05) && (!p06)) ||
	    (p03 && (!p04) && p05 && (!p07)) ||
	    (p03 && (!p04) && (!p05) && p07) ||
	    ((!p03) && p04 && p05 && (!p11)) ||
	    ((!p03) && p04 && (!p05) && p11) ||
	    ((!p03) && (!p04)));
	dotrace = true;
	dotrace = true;
	dotrace = true;
	dotrace = true;
	dotrace = true;
	bool p12 =
	    !((p02 && p03 && (!p06)) ||
	    (p02 && (!p03)) ||
	    ((!p02) && p03));

	char out19 = p19 ? '1' : '0';
	pin19 = AS(p19);
	char out18;
	if(p02) {
		out18 = p18 ? '1' : '0';
		pin18 = AS(p18);
	} else {
		out18 = 'Z';
		pin18 = sc_logic_Z;
	}
	char out17;
	if((!p02)) {
		out17 = p17 ? '1' : '0';
		pin17 = AS(p17);
	} else {
		out17 = 'Z';
		pin17 = sc_logic_Z;
	}
	char out12 = p12 ? '1' : '0';
	pin12 = AS(p12);

	if (dotrace) {
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
		    <<out12
		    <<out17
		    <<out18
		    <<out19
		);
	}
}
