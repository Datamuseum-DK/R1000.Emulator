#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "F381.hh"

struct scm_f381_state {
	struct ctx ctx;
};

void
SCM_F381 :: loadit(const char *arg)
{
	state = (struct scm_f381_state *)
	    CTX_Get("f381", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F381 :: doit(void)
{
	unsigned a = 0, b = 0, c = 0, sel = 0, r, p = 1, g = 1;

	state->ctx.activations++;
	if (IS_H(pin3)) a |= 1;
	if (IS_H(pin1)) a |= 2;
	if (IS_H(pin19)) a |= 4;
	if (IS_H(pin17)) a |= 8;
	if (IS_H(pin4)) b |= 1;
	if (IS_H(pin2)) b |= 2;
	if (IS_H(pin18)) b |= 4;
	if (IS_H(pin16)) b |= 8;
	if (IS_H(pin5)) sel |= 1;
	if (IS_H(pin6)) sel |= 2;
	if (IS_H(pin7)) sel |= 4;
	if (IS_H(pin15)) c |= 1;

	switch (sel) {
	case 0: r = 0; break;
	case 1: r = 0xf + c + b - a; break;
	case 2: r = 0xf + c + a - b; break;
	case 3: r = a + b + c; break;
	case 4: r = a ^ b; break;
	case 5: r = a | b; break;
	case 6: r = a & b; break;
	case 7: r = 0xf; break;
	}

	if (c && r == 0x10)
		p = 0;
	else if (r == 0xf && sel < 4)
		p = c;
	else if (r == 0xf)
		p = 0;
	else if (r > 0xf)
		g = 0;

	pin8 = AS(r & 1);
	pin9 = AS(r & 2);
	pin11 = AS(r & 4);
	pin12 = AS(r & 8);
	pin13 = AS(g);
	pin14 = AS(p);

	r &= 0xf;

	TRACE(
	    << " a " << a
	    << " b " << b
	    << " c " << c
	    << " sel " << sel
	    << " r " << r
	    << " p " << p
	    << " g " << g
	    << " pins "
	    <<pin1
	    <<pin2
	    <<pin3
	    <<pin4
	    <<pin5
	    <<pin6
	    <<pin7
	    <<pin15
	    <<pin16
	    <<pin17
	    <<pin18
	    <<pin19
	);
}
