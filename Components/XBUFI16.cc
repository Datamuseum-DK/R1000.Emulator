#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XBUFI16.hh"

struct scm_xbufi16_state {
	struct ctx ctx;
	int job;
	bool out[64];
};

SCM_XBUFI16 :: SCM_XBUFI16(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);

	#define PIN(bit, pin_in, pin_out) << pin_in
	sensitive << pin1 XBUFI16_PINS();
	#undef PIN

	state = (struct scm_xbufi16_state *)CTX_Get("XBUFI16", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	state->job = -1;
}

#define XBUF_CLASS SCM_XBUFI16
#define XBUF_PINS XBUFI16_PINS
#define XBUF_SENSE IS_L

#include "Components/xbuf_doit.h"
