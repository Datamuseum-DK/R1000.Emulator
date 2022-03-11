#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS

#include "XREG20.hh"

struct scm_xreg20_state {
	struct ctx ctx;
	uint64_t data;
	int job;
};

SCM_XREG20 :: SCM_XREG20(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << PIN_OE << PIN_CLK.pos();

	state = (struct scm_xreg20_state *)CTX_Get("XREG20", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XREG20 :: doit(void)
{

	#include "XREG_doit.hh"
}
