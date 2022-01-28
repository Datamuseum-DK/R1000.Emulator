#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F85.hh"

// 4-bit magnitude comparator
// Philips IC15 1994 Sep 27

struct scm_f85_state {
	struct ctx ctx;
};

void
SCM_F85 :: loadit(const char *arg)
{
	state = (struct scm_f85_state *)
	    CTX_Get("f85", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F85 :: doit(void)
{
	unsigned a = 0, b = 0, o = 0;

	state->ctx.activations++;
	if (IS_H(pin10)) a += 1;
	if (IS_H(pin12)) a += 2;
	if (IS_H(pin13)) a += 4;
	if (IS_H(pin15)) a += 8;
	if (IS_H(pin9)) b += 1;
	if (IS_H(pin11)) b += 2;
	if (IS_H(pin14)) b += 4;
	if (IS_H(pin1)) b += 8;
	if (a > b) {
		o = 0x4;
	} else if (a < b) {
		o = 0x1;
	} else {
		if (IS_L(pin2) && IS_L(pin3))
			o |= 0x4;
		if (IS_H(pin3))
			o |= 0x2;
		if (IS_L(pin3) && IS_L(pin4))
			o |= 0x1;
	}
	TRACE(
	    << " a " << a
	    << " " << pin15 << pin13 << pin12 << pin10
	    << " b " << b
	    << " " << pin1 << pin14 << pin11 << pin9
	    << " e< " << pin2
	    << " e= " << pin3
	    << " e> " << pin4
	    << " | " << (o & 4 ? ">" : "")
	    << (o & 2 ? "=" : "")
	    << (o & 1 ? "<" : "")
	);
	pin5 = AS(o & 4);
	pin6 = AS(o & 2);
	pin7 = AS(o & 1);
}
