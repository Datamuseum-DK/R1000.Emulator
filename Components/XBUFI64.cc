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

void
SCM_XBUFI64 :: doit(void)
{
	state->ctx.activations++;


	#define PIN(bit,pin_in,pin_out) << pin_in
	if (state->job || (state->ctx.do_trace & 2)) {
		TRACE(<< "job " << state->job << " e " << pin1
		    << " d " XBUFI64_PINS());
	}
	#undef PIN
	if (state->job) {
		#define PIN(bit,pin_in,pin_out) pin_out = AS(state->out[bit]);
		XBUFI64_PINS()
		#undef PIN
		state->job = 0;
	}

	if (IS_H(pin1)) {
		#define PIN(bit,pin_in,pin_out) \
		pin_out = sc_logic_Z;
		XBUFI64_PINS()
		#undef PIN
		next_trigger(pin1.negedge_event());
	} else {
		bool tmp;
		#define PIN(bit,pin_in,pin_out) \
		tmp = IS_L(pin_in); \
		if (tmp != state->out[bit]) { \
			state->job = 1; \
			state->out[bit] = tmp; \
		}
		XBUFI64_PINS()
		#undef PIN
		if (state->job)
			next_trigger(5, SC_NS);
	}
}
