// Generated from ../_Firmware/TPARGAL-02.BIN by gal16v8.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "TPARPAL.hh"

struct scm_tparpal_state {
	struct ctx ctx;
};

void
SCM_TPARPAL :: loadit(const char *arg)
{
	state = (struct scm_tparpal_state *)
	    CTX_Get("tparpal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_TPARPAL :: doit(void)
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
	    !((p04 && p05) ||
	    (p02 && (!p04)) ||
	    ((!p03) && (!p04)) ||
	    ((!p01) && (!p04) && (!p07) && (!p09)) ||
	    ((!p04) && (!p06) && (!p07) && (!p08) && (!p09)));
	dotrace = true;
	bool p18 =
	    !(((!p03)) ||
	    ((!p04) && p07) ||
	    ((!p04) && p09));
	dotrace = true;
	dotrace = true;
	bool p16 =
	    !((p04) ||
	    ((!p02) && (!p03)) ||
	    ((!p02) && (!p01) && (!p07) && (!p09)) ||
	    ((!p02) && (!p06) && (!p07) && (!p08) && (!p09)));
	dotrace = true;
	bool p15 =
	    !(((!p06)) ||
	    ((!p01) && p04) ||
	    ((!p01) && p03));
	dotrace = true;
	bool p14 =
	    !(((!p08)) ||
	    ((!p01) && p04) ||
	    ((!p01) && p03));
	dotrace = true;
	bool p13 =
	    !(((!p02) && p04 && (!p11)) ||
	    ((!p02) && (!p03) && (!p11)) ||
	    ((!p02) && (!p07) && (!p09) && (!p11)));
	dotrace = true;
	bool p12 =
	    !(((!p03)) ||
	    (p04 && p05) ||
	    (p02 && (!p04)) ||
	    ((!p01) && (!p04) && (!p07) && (!p09)) ||
	    ((!p04) && (!p06) && (!p07) && (!p08) && (!p09)));

	char out19 = p19 ? '1' : '0';
	pin19 = AS(p19);
	char out18 = p18 ? '1' : '0';
	pin18 = AS(p18);
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
		    <<out18
		    <<out19
		);
	}
}
