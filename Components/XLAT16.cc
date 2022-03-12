#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS

#include "XLAT16.hh"

struct scm_xlat16_state {
	struct ctx ctx;
	uint16_t data;
	int job;
};

SCM_XLAT16 :: SCM_XLAT16(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	#define PINM(bit, pin_in, pin_out) << pin_in
	sensitive << PIN_LE << PIN_OE PIN_PAIRS(PINM);
	#undef PINM

	state = (struct scm_xlat16_state *)CTX_Get("XLAT16", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XLAT16 :: doit(void)
{

	#include "XLAT_doit.hh"
}
