#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "F151.hh"

// 8-Input Multiplexer
// Fairchild DS09481 April 1988 Revised September 2000

struct scm_f151_state {
	struct ctx ctx;
};

void
SCM_F151 :: loadit(const char *arg)
{
	state = (struct scm_f151_state *)
	    CTX_Get("f151", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F151 :: doit(void)
{
	unsigned adr = 0;
	state->ctx.activations++;
	if (IS_H(pin11)) adr |= 1;
	if (IS_H(pin10)) adr |= 2;
	if (IS_H(pin9)) adr |= 4;
	if (!IS_L(pin7)) adr |= 8;
	bool s;
	switch (adr) {
	case 0: s = IS_H(pin4); break;
	case 1: s = IS_H(pin3); break;
	case 2: s = IS_H(pin2); break;
	case 3: s = IS_H(pin1); break;
	case 4: s = IS_H(pin15); break;
	case 5: s = IS_H(pin14); break;
	case 6: s = IS_H(pin13); break;
	case 7: s = IS_H(pin12); break;
	default: s = false ; break;
	}
	TRACE(
	    << pin1
	    << pin2
	    << pin3
	    << pin4
	    << pin7
	    << pin9
	    << pin10
	    << pin11
	    << pin12
	    << pin13
	    << pin14
	    << pin15
	    << "|"
	    << adr
	    << s
	);
	pin5 = AS(s);
	pin6 = AS(!s);
}
