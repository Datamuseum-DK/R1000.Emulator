#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XBUF64.hh"

struct scm_xbuf64_state {
	struct ctx ctx;
	int job;
	bool out[64];
};

SCM_XBUF64 :: SCM_XBUF64(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);

	#define PIN(bit, pin_in, pin_out) << pin_in
	sensitive << pin1 XBUF64_PINS();
	#undef PIN

	state = (struct scm_xbuf64_state *)CTX_Get("XBUF64", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	state->job = -1;
}

#define XBUF_CLASS SCM_XBUF64
#define XBUF_PINS XBUF64_PINS
#define XBUF_SENSE IS_H

#include "Components/xbuf_doit.h"
