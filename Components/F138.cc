#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "F138.hh"

// 1-of-8 decoder/demultiplexer
// Philips IC15 1991 Feb 14

struct scm_f138_state {
	struct ctx ctx;
};

void
SCM_F138 :: loadit(const char *arg)
{
	state = (struct scm_f138_state *)
	    CTX_Get("f138", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F138 :: doit(void)
{
	unsigned adr = 0;
	state->ctx.activations++;
	if (IS_H(pin1)) adr |= 1;
	if (IS_H(pin2)) adr |= 2;
	if (IS_H(pin3)) adr |= 4;
	if (!IS_L(pin4)) adr |= 8;
	if (!IS_L(pin5)) adr |= 8;
	if (!IS_H(pin6)) adr |= 8;
	TRACE(
	    << " s "
	    << pin3 << pin2 << pin1
	    << " e "
	    << pin4 << pin5 << pin6
	    << " | "
	    << std::hex << adr
	);
	pin7 = AS(adr != 7);
	pin9 = AS(adr != 6);
	pin10 = AS(adr != 5);
	pin11 = AS(adr != 4);
	pin12 = AS(adr != 3);
	pin13 = AS(adr != 2);
	pin14 = AS(adr != 1);
	pin15 = AS(adr != 0);
}
