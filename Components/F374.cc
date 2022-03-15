// Octal D-Type Flip-Flop with 3-STATE Outputs
// Fairchild DS009524 May 1988 Revised September 2000

#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS
#include "F374.hh"

struct scm_f374_state {
	struct ctx ctx;
	uint64_t data;
	int job;
};

SCM_F374 :: SCM_F374(sc_module_name nm, const char *arg) : sc_module(nm)
{
	state = (struct scm_f374_state *)
	    CTX_Get("f374", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);

	SC_METHOD(doit);
	sensitive << PIN_OE << PIN_CLK.pos();
}

void
SCM_F374 :: doit(void)
{

	#include "XREG_doit.hh"
}
