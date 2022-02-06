#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XBUFI32.hh"

struct scm_xbufi32_state {
	struct ctx ctx;
};

SCM_XBUFI32 :: SCM_XBUFI32(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);

	#define PIN(bit, pin_in, pin_out) \
	<< pin_in
	sensitive << pin1 XBUFI32_PINS();
	#undef PIN

	state = (struct scm_xbufi32_state *)CTX_Get("XBUFI32", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XBUFI32 :: doit(void)
{
	state->ctx.activations++;

	#define PIN(bit,pin_in,pin_out) \
	<< pin_in
	TRACE(<< " e " << pin1 << " d " XBUFI32_PINS());
	#undef PIN

	if (IS_H(pin1)) {

		#define PIN(bit,pin_in,pin_out) \
		pin_out = sc_logic_Z;
		XBUFI32_PINS()
		#undef PIN

	} else {

		#define PIN(bit,pin_in,pin_out) \
		pin_out = AS(IS_L(pin_in));
		XBUFI32_PINS()
		#undef PIN

	}
}
