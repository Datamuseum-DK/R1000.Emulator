#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F153.hh"

// Dual 4-Input Multiplexer
// Fairchild DS009482 April 1988 Revised September 2000

struct scm_f153_state {
	struct ctx ctx;
};

void
SCM_F153 :: loadit(const char *arg)
{
	state = (struct scm_f153_state *)
	    CTX_Get("f153", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F153 :: doit(void)
{
	bool s[2];
	state->ctx.activations++;
	if (IS_L(pin14) && IS_L(pin2)) {
		s[0] = IS_H(pin6);
		s[1] = IS_H(pin10);
	} else if (IS_H(pin14) && IS_L(pin2)) {
		s[0] = IS_H(pin5);
		s[1] = IS_H(pin11);
	} else if (IS_L(pin14) && IS_H(pin2)) {
		s[0] = IS_H(pin4);
		s[1] = IS_H(pin12);
	} else {
		s[0] = IS_H(pin3);
		s[1] = IS_H(pin13);
	}
	if (IS_H(pin1))
		s[0] = false;
	if (IS_H(pin15))
		s[1] = false;
	TRACE(
	    << " a " << pin6 << pin5 << pin4 << pin3
	    << " b " << pin10 << pin11 << pin12 << pin13
	    << " e " << pin1 << pin15
	    << " s " << pin2 << pin14
	    << " | " << s[0] << s[1]
	);
	pin7 = AS(s[0]);
	pin9 = AS(s[1]);
}
