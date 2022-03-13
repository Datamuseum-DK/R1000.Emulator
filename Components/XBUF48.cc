#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS

#include "XBUF48.hh"

struct scm_xbuf48_state {
	struct ctx ctx;
	uint64_t data;
	int job;
};

SCM_XBUF48 :: SCM_XBUF48(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	#define PINM(bit, pin_in, pin_out) << pin_in
	sensitive << PIN_OE PIN_PAIRS(PINM);
	#undef PINM

	state = (struct scm_xbuf48_state *)CTX_Get("XBUF48", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XBUF48 :: doit(void)
{

	#include "XBUF_doit.hh"
}
