#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS

#include "XREG32.hh"

struct scm_xreg32_state {
	struct ctx ctx;
	uint64_t data;
	int job;
};

SCM_XREG32 :: SCM_XREG32(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << PIN_OE << PIN_CLK.pos();

	state = (struct scm_xreg32_state *)CTX_Get("XREG32", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XREG32 :: doit(void)
{

	#include "XREG_doit.hh"
}
