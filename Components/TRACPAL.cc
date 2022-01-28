// Generated from ../_Firmware/TRACGAL-01.BIN by gal16v8.py
#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "TRACPAL.hh"

struct scm_tracpal_state {
	struct ctx ctx;
};

void
SCM_TRACPAL :: loadit(const char *arg)
{
	state = (struct scm_tracpal_state *)
	    CTX_Get("tracpal", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_TRACPAL :: doit(void)
{

	state->ctx.activations++;

	bool p02 = IS_H(pin2);
	bool p03 = IS_H(pin3);
	bool p04 = IS_H(pin4);
	bool p05 = IS_H(pin5);
	bool p06 = IS_H(pin6);
	bool p07 = IS_H(pin7);
	bool dotrace = false;
	dotrace = true;
	bool p23 =
	    ((!p03)) ||
	    ((!p05) && p04 && p06) ||
	    ((!p04) && (!p05) && (!p06));
	dotrace = true;
	bool p22 =
	    !(((!p02) && (!p03) && p04) ||
	    ((!p05) && p03 && p04 && p06));
	dotrace = true;
	bool p21 =
	    ((!p07)) ||
	    ((!p04) && p03 && p05);
	dotrace = true;
	bool p20 =
	    ((!p07)) ||
	    ((!p03)) ||
	    (p04 && p05 && p06) ||
	    ((!p05) && (!p06) && p04);
	dotrace = true;
	bool p19 =
	    (p02 && p03 && p04 && p06) ||
	    (p03 && p04 && p05 && p06);
	dotrace = true;
	bool p18 =
	    !(((!p04) && p03 && p05 && p06));
	dotrace = true;
	bool p17 =
	    ((!p07)) ||
	    ((!p04) && (!p05) && (!p06) && p03) ||
	    ((!p02) && (!p05) && p03 && p04 && p06);
	dotrace = true;
	bool p16 =
	    !(((!p03) && p05) ||
	    ((!p05) && p02 && p03 && p04 && p06) ||
	    ((!p04) && (!p05) && (!p06) && p02 && p03));
	dotrace = true;
	bool p15 =
	    !(((!p03) && (!p06) && p05) ||
	    ((!p03) && (!p05) && p06) ||
	    ((!p04) && (!p05) && (!p06) && p02 && p03));
	dotrace = true;
	bool p14 =
	    !(((!p04) && (!p05) && (!p06) && p03));

	char out23 = p23 ? '1' : '0';
	pin23 = AS(p23);
	char out22 = p22 ? '1' : '0';
	pin22 = AS(p22);
	char out21 = p21 ? '1' : '0';
	pin21 = AS(p21);
	char out20 = p20 ? '1' : '0';
	pin20 = AS(p20);
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

	if (dotrace) {
		TRACE(
		    <<pin2
		    <<pin3
		    <<pin4
		    <<pin5
		    <<pin6
		    <<pin7
		    << " | "
		    <<out14
		    <<out15
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
