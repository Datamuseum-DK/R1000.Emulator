#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "8641.hh"

// DS8641 Quad Unified Bus Tranceiver
// National Semiconductor TL/F/5806  RRD-B30M36  January 1996

struct scm_8641_state {
	struct ctx ctx;
};

void
SCM_8641 :: loadit(const char *arg)
{
	state = (struct scm_8641_state *)CTX_Get("8641", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_8641 :: doit(void)
{

	state->ctx.activations++;

	TRACE(
	    << " e_ "
	    <<pin7
	    <<pin9
	    << " b "
	    <<pin15
	    <<pin12
	    <<pin1
	    <<pin4
	    << " i "
	    <<pin14
	    <<pin11
	    <<pin2
	    <<pin5
	);
	if (IS_L(pin7) && IS_L(pin9)) {
		pin15 = AS(IS_L(pin14));
		pin12 = AS(IS_L(pin11));
		pin1 = AS(IS_L(pin2));
		pin4 = AS(IS_L(pin5));
	} else {
		pin15 = sc_logic_Z;
		pin12 = sc_logic_Z;
		pin1 = sc_logic_Z;
		pin4 = sc_logic_Z;
	}
	pin13 = AS(IS_L(pin15));
	pin10 = AS(IS_L(pin12));
	pin3 = AS(IS_L(pin1));
	pin6 = AS(IS_L(pin4));
}
