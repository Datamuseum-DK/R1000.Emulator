#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XBUFI64.hh"

struct scm_xbufi64_state {
	struct ctx ctx;
	int job;
	bool out[64];
};

SCM_XBUFI64 :: SCM_XBUFI64(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);

	#define PIN(bit, pin_in, pin_out) << pin_in
	sensitive << pin1 XBUFI64_PINS();
	#undef PIN

	state = (struct scm_xbufi64_state *)CTX_Get("XBUFI64", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

#define XBUF_CLASS SCM_XBUFI64
#define XBUF_PINS XBUFI64_PINS
#define XBUF_SENSE IS_L

#include "Components/xbuf_doit.h"
