#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS

#include "XBUF16.hh"

struct scm_xbuf16_state {
	struct ctx ctx;
	uint16_t data;
	int job;
};

SCM_XBUF16 :: SCM_XBUF16(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	#define PINM(bit, pin_in, pin_out) << pin_in
	sensitive << PIN_OE_ PIN_PAIRS(PINM);
	#undef PINM

	state = (struct scm_xbuf16_state *)CTX_Get("XBUF16", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XBUF16 :: doit(void)
{

	#include "XBUF_doit.hh"
}
