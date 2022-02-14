#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XBUF32.hh"

struct scm_xbuf32_state {
	struct ctx ctx;
	int job;
	bool out[64];
};

SCM_XBUF32 :: SCM_XBUF32(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);

	#define PIN(bit, pin_in, pin_out) << pin_in
	sensitive << pin1 XBUF32_PINS();
	#undef PIN

	state = (struct scm_xbuf32_state *)CTX_Get("XBUF32", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

#define XBUF_CLASS SCM_XBUF32
#define XBUF_PINS XBUF32_PINS
#define XBUF_SENSE IS_H

#include "Components/xbuf_doit.h"
