#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XBUFI64.hh"

struct scm_XBUFI64_state {
	struct ctx ctx;
	unsigned slowstart;
	bool out;
};

SCM_XBUFI64 :: SCM_XBUFI64(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);

	#define PIN(bit, pin_in, pin_out) \
	<< pin_in
	sensitive << pin1 XBUFI64_PINS();
	#undef PIN

	state = (struct scm_XBUFI64_state *)CTX_Get("XBUFI64", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	state->slowstart = 100;
}

void
SCM_XBUFI64 :: doit(void)
{
	state->ctx.activations++;
	if (state->slowstart) {
		state->slowstart--;
		return;
	}
	bool z = IS_H(pin1);

	#define PIN(bit,pin_in,pin_out) << pin_in
	if ((state->ctx.do_trace & 2) || !z || state->out)
		TRACE(<< " e " << pin1 << " d " XBUFI64_PINS());
	#undef PIN

	if (z) {

		#define PIN(bit,pin_in,pin_out) \
		pin_out = sc_logic_Z;
		XBUFI64_PINS()
		#undef PIN
		state->out = false;

	} else {

		#define PIN(bit,pin_in,pin_out) \
		pin_out = AS(IS_L(pin_in));
		XBUFI64_PINS()
		#undef PIN
		state->out = true;

	}
}
