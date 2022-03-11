#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS

#include "XLAT64.hh"

struct scm_xlat64_state {
	struct ctx ctx;
	uint64_t data;
	int job;
};

SCM_XLAT64 :: SCM_XLAT64(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	#define PINM(bit, pin_in, pin_out) << pin_in
	sensitive << PIN_LE << PIN_OE PIN_PAIRS(PINM);
	#undef PINM

	state = (struct scm_xlat64_state *)CTX_Get("XLAT64", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XLAT64 :: doit(void)
{

	#include "XLAT_doit.hh"
}
