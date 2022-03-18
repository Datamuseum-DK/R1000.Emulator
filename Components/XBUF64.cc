#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS

#include "XBUF64.hh"

struct scm_xbuf64_state {
	struct ctx ctx;
	uint64_t data;
	int job;
};

SCM_XBUF64 :: SCM_XBUF64(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	#define PINM(bit, pin_in, pin_out) << pin_in
	sensitive << PIN_OE_ PIN_PAIRS(PINM);
	#undef PINM

	state = (struct scm_xbuf64_state *)CTX_Get("XBUF64", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XBUF64 :: doit(void)
{

	#include "XBUF_doit.hh"
}
