#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F148.hh"

// 8-Line to 3-Line Priority Encoder
// Fairchild DS009480 April 1988 Revised September 2000

struct scm_f148_state {
	struct ctx ctx;
};

void
SCM_F148 :: loadit(const char *arg)
{
	state = (struct scm_f148_state *)
	    CTX_Get("f148", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F148 :: doit(void)
{
	unsigned s;
	state->ctx.activations++;
	if (IS_L(pin4))
		s = 7;
	else if (IS_L(pin3))
		s = 6;
	else if (IS_L(pin2))
		s = 5;
	else if (IS_L(pin1))
		s = 4;
	else if (IS_L(pin13))
		s = 3;
	else if (IS_L(pin12))
		s = 2;
	else if (IS_L(pin11))
		s = 1;
	else if (IS_L(pin10))
		s = 0;
	else
		s = 8|16;
	if (IS_H(pin5))
		s = 16;
	TRACE(
	    << pin1
	    << pin2
	    << pin3
	    << pin4
	    << pin5
	    << pin10
	    << pin11
	    << pin12
	    << pin13
	    << "|"
	    << std::hex << s
	);
	pin9 = AS(!(s & 1));
	pin7 = AS(!(s & 2));
	pin6 = AS(!(s & 4));
	pin14 = AS(s & 16);
	pin15 = AS(!(s & 8));
}
