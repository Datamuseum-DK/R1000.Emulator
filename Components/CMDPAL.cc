// Generated from ../_Firmware/CMDGAL-02.BIN by gal16v8.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "CMDPAL.hh"

struct scm_cmdpal_state {
	struct ctx ctx;
};

void
SCM_CMDPAL :: loadit(const char *arg)
{
	state = (struct scm_cmdpal_state *)
	    CTX_Get("cmdpal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_CMDPAL :: doit(void)
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
	bool p13 = IS_H(pin13);
	bool dotrace = false;
	dotrace = true;
	bool p19 =
	    !(((!p01) && p07 && p13 && p09) ||
	    (p01 && p07 && p13 && p11) ||
	    (p02 && p13));
	dotrace = true;
	bool p18 =
	    !(((!p13)) ||
	    (p01 && p07 && p11) ||
	    ((!p01) && p07 && p09) ||
	    (p04 && p13));
	dotrace = true;
	bool p17 =
	    !(((!p01) && p07 && p13 && p09) ||
	    (p01 && p07 && p13 && p11) ||
	    (p05 && p13));
	dotrace = true;
	bool p16 =
	    !(((!p02) && (!p01) && (!p07) && p13 && p09) ||
	    ((!p01) && (!p03) && (!p07) && p13 && p09) ||
	    ((!p01) && (!p04) && (!p07) && p13 && p09) ||
	    ((!p01) && (!p05) && (!p07) && p13 && p09) ||
	    ((!p01) && (!p13) && p09) ||
	    ((!p06) && (!p07) && (!p09)) ||
	    (p01 && (!p09)));
	dotrace = true;
	bool p15 =
	    !(((!p02) && (!p01) && (!p07) && p13 && p09) ||
	    ((!p01) && (!p03) && (!p07) && p13 && p09) ||
	    ((!p01) && (!p04) && (!p07) && p13 && p09) ||
	    ((!p01) && (!p05) && (!p07) && p13 && p09) ||
	    ((!p01) && (!p13) && p09) ||
	    (p01 && (!p08)));
	dotrace = true;
	bool p14 =
	    !(((!p01) && (!p09)) ||
	    (p01 && (!p11)));
	dotrace = true;
	dotrace = true;
	bool p12 =
	    !(((!p13)) ||
	    (p01 && p07 && p11) ||
	    ((!p01) && p07 && p09) ||
	    (p03 && p13));

	char out19 = p19 ? '1' : '0';
	pin19 = AS(p19);
	char out18 = p18 ? '1' : '0';
	pin18 = AS(p18);
	char out17 = p17 ? '1' : '0';
	pin17 = AS(p17);
	char out16 = p16 ? '1' : '0';
	pin16 = AS(p16);
	char out15 = p15 ? '1' : '0';
	pin15 = AS(p15);
	char out14 = p14 ? '1' : '0';
	pin14 = AS(p14);
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
		    <<pin13
		    << " | "
		    <<out12
		    <<out14
		    <<out15
		    <<out16
		    <<out17
		    <<out18
		    <<out19
		);
	}
}
