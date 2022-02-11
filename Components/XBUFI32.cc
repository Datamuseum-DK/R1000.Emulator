#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XBUFI32.hh"

struct scm_xbufi32_state {
	struct ctx ctx;
	int job;
	bool out[64];
};

SCM_XBUFI32 :: SCM_XBUFI32(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);

	#define PIN(bit, pin_in, pin_out) << pin_in
	sensitive << pin1 XBUFI32_PINS();
	#undef PIN

	state = (struct scm_xbufi32_state *)CTX_Get("XBUFI32", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

#define XBUF_CLASS SCM_XBUFI32
#define XBUF_PINS XBUFI32_PINS
#define XBUF_SENSE IS_L

#include "Components/xbuf_doit.h"
