#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "F139.hh"

// (Dual) 1-of-4 Decoder/Demultiplexer
// Fairchild DS009479 April 1988 Revised September 2000

struct scm_f139_state {
	struct ctx ctx;
};

void
SCM_F139 :: loadit(const char *arg)
{
	state = (struct scm_f139_state *)
	    CTX_Get("f139", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F139 :: doit(void)
{
	unsigned adr = 0;
	state->ctx.activations++;
	if (IS_H(pin2)) adr |= 1;
	if (IS_H(pin3)) adr |= 2;
	if (IS_H(pin1)) adr |= 4;
	TRACE(
	    << "e " << pin1
	    << " i " << pin3 << pin2
	    << " o " << adr
	);
	pin4 = AS(adr != 0);
	pin5 = AS(adr != 1);
	pin6 = AS(adr != 2);
	pin7 = AS(adr != 3);
}
