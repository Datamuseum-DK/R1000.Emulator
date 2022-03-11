#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS

#include "XREG64.hh"

struct scm_xreg64_state {
	struct ctx ctx;
	uint64_t data;
	int job;
};

SCM_XREG64 :: SCM_XREG64(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << PIN_OE << PIN_CLK.pos();

	state = (struct scm_xreg64_state *)CTX_Get("XREG64", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XREG64 :: doit(void)
{

	#include "XREG_doit.hh"
}
