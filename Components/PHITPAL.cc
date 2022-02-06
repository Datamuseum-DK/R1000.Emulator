// Generated from ../_Firmware/PHITGAL-02.BIN by gal16v8.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "PHITPAL.hh"

struct scm_phitpal_state {
	struct ctx ctx;
};

void
SCM_PHITPAL :: loadit(const char *arg)
{
	state = (struct scm_phitpal_state *)
	    CTX_Get("phitpal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_PHITPAL :: doit(void)
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
	    !(((!p02) && (!p01) && (!p03) && (!p04) && (!p05) && p06 && (!p08)) ||
	    ((!p02) && (!p01) && p03 && (!p04) && (!p05) && (!p06) && (!p08)) ||
	    ((!p02) && p01 && (!p03) && (!p04) && p05 && p06 && (!p08)) ||
	    ((!p02) && p01 && p03 && (!p04) && p05 && (!p06) && (!p08)) ||
	    (p07 && p08 && p11));
	dotrace = true;
	bool p18 =
	    !((p02 && (!p01) && (!p03) && (!p04) && (!p05) && p06 && (!p08)) ||
	    (p02 && (!p01) && p03 && (!p04) && (!p05) && (!p06) && (!p08)) ||
	    (p02 && p01 && (!p03) && (!p04) && p05 && p06 && (!p08)) ||
	    (p02 && p01 && p03 && (!p04) && p05 && (!p06) && (!p08)) ||
	    (p07 && p08 && p09));
	dotrace = true;
	bool p17 =
	    !((p02 && (!p01) && (!p03) && p04 && (!p05) && p06 && (!p08)) ||
	    (p02 && (!p01) && p03 && p04 && (!p05) && (!p06) && (!p08)) ||
	    (p02 && p01 && (!p03) && p04 && p05 && p06 && (!p08)) ||
	    (p02 && p01 && p03 && p04 && p05 && (!p06) && (!p08)) ||
	    (p07 && p08 && p09));
	dotrace = true;
	bool p16 =
	    !(((!p02) && p01 && p05) ||
	    ((!p02) && (!p01) && (!p05)));
	dotrace = true;
	bool p15 =
	    !((p02 && p01 && p05) ||
	    (p02 && (!p01) && (!p05)));
	dotrace = true;
	bool p14 =
	    !(((!p01) && p03 && (!p04) && (!p05)) ||
	    (p01 && p03 && (!p04) && p05));
	dotrace = true;
	bool p13 =
	    !((p07 && p08));
	dotrace = true;
	bool p12 =
	    !(((!p02) && (!p01) && (!p03) && p04 && (!p05) && p06 && (!p08)) ||
	    ((!p02) && (!p01) && p03 && p04 && (!p05) && (!p06) && (!p08)) ||
	    ((!p02) && p01 && (!p03) && p04 && p05 && p06 && (!p08)) ||
	    ((!p02) && p01 && p03 && p04 && p05 && (!p06) && (!p08)) ||
	    (p07 && p08 && p11));

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
	char out13 = p13 ? '1' : '0';
	pin13 = AS(p13);
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
		    <<out13
		    <<out14
		    <<out15
		    <<out16
		    <<out17
		    <<out18
		    <<out19
		);
	}
}
