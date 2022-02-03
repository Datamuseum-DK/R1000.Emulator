#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XBUF64.hh"

struct scm_XBUF64_state {
	struct ctx ctx;
	bool out;
	unsigned startup;
};

SCM_XBUF64 :: SCM_XBUF64(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);

	#define PIN(bit, pin_in, pin_out) \
	<< pin_in
	sensitive << pin1 XBUF64_PINS();
	#undef PIN

	state = (struct scm_XBUF64_state *)CTX_Get("XBUF64", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	state->startup = 1;
}

void
SCM_XBUF64 :: doit(void)
{
	state->ctx.activations++;
	bool z = IS_H(pin1);

	if (state->startup) {
		next_trigger(5, SC_NS);
		state->startup = 0;
		return;
	}

	#define PIN(bit,pin_in,pin_out) << pin_in
	if ((state->ctx.do_trace & 2) || !z || state->out)
		TRACE(<< " e " << pin1 << " d " XBUF64_PINS());
	#undef PIN

	if (z) {

		#define PIN(bit,pin_in,pin_out) \
		pin_out = sc_logic_Z;
		XBUF64_PINS()
		#undef PIN
		state->out = false;

	} else {

		#define PIN(bit,pin_in,pin_out) \
		pin_out = AS(IS_H(pin_in));
		XBUF64_PINS()
		#undef PIN
		state->out = true;

	}
}
