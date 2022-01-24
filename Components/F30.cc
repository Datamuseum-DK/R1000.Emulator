#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "F30.hh"

struct scm_f30_state {
	struct ctx ctx;
};

void
SCM_F30 :: loadit(const char *arg)
{
	state = (struct scm_f30_state *)
	    CTX_Get("f30", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F30 :: doit(void)
{

	state->ctx.activations++;
	bool s = !(
	    IS_H(pin1) && IS_H(pin2) && IS_H(pin3) && IS_H(pin4)
	    && IS_H(pin5) && IS_H(pin6) && IS_H(pin11) && IS_H(pin12)
	);
	TRACE(
	    << pin11
	    << pin12
	    << pin1
	    << pin2
	    << pin3
	    << pin4
	    << pin5
	    << pin6
	    << "|"
	    << s
	);
	pin8 = AS(s);
}
