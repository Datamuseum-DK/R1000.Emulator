#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "F163.hh"

// SYNCHRONOUS 4-BIT BINARY COUNTER
// Fairchild Rev 1.0.2 April 2007

struct scm_f163_state {
	struct ctx ctx;
	unsigned state;
};

void
SCM_F163 :: loadit(const char *arg)
{
	state = (struct scm_f163_state *)
	    CTX_Get("f163", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F163 :: doit(void)
{
	state->ctx.activations++;
	if (IS_L(pin1)) {
		state->state = 0;
	} else if (IS_L(pin9)) {
		state->state = 0;
		if (IS_H(pin3)) state->state |= 1;
		if (IS_H(pin4)) state->state |= 2;
		if (IS_H(pin5)) state->state |= 4;
		if (IS_H(pin6)) state->state |= 8;
	} else if (IS_H(pin7) && IS_H(pin10)) {
		state->state = (state->state + 1) & 0xf;
	}
	TRACE(
	    << " clr " << pin1
	    << " clk " << pin2
	    << " d " << pin6 << pin5 << pin4 << pin3
	    << " enp " << pin7
	    << " ld " << pin9
	    << " ent " << pin10
	    << "|"
	    << std::hex
	    << state->state
	);
	pin14 = AS(state->state & 1);
	pin13 = AS(state->state & 2);
	pin12 = AS(state->state & 4);
	pin11 = AS(state->state & 8);
	pin15 = AS((state->state == 0xf) && IS_H(pin10));
}
