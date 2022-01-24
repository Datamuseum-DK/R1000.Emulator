#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "F280.hh"

// 9-Bit Parity Generator Checker
// Fairchild DS009512 April 1988 Revised September 2000

struct scm_f280_state {
	struct ctx ctx;
};

void
SCM_F280 :: loadit(const char *arg)
{
	state = (struct scm_f280_state *)
	    CTX_Get("f280", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F280 :: doit(void)
{

	state->ctx.activations++;
	bool s = IS_H(pin1)
	    ^ IS_H(pin2)
	    ^ IS_H(pin4)
	    ^ IS_H(pin8)
	    ^ IS_H(pin9)
	    ^ IS_H(pin10)
	    ^ IS_H(pin11)
	    ^ IS_H(pin12)
	    ^ IS_H(pin13);
	TRACE(
	    << pin8
	    << pin9
	    << pin10
	    << pin11
	    << pin12
	    << pin13
	    << pin1
	    << pin2
	    << pin4
	    << " odd "
	    << s
	);
	pin5 = AS(!s);
	pin6 = AS(s);
}
