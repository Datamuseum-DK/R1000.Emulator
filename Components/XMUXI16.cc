#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XMUXI16.hh"

struct scm_xmuxi16_state {
	struct ctx ctx;
};

SCM_XMUXI16 :: SCM_XMUXI16(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	#define PIN(bit, pin_a, pin_b, pin_out) << pin_a << pin_b
	sensitive << pin1 << pin2 XMUXI16_PINS();
	#undef PIN

	state = (struct scm_xmuxi16_state *)CTX_Get("XMUXI16", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XMUXI16 :: doit(void)
{
	state->ctx.activations++;

	if (IS_H(pin2)) {
		TRACE( << " Z ");
		#define PIN(bit, pin_a, pin_b, pin_out) pin_out = sc_logic_Z;
		XMUXI16_PINS()
		#undef PIN
		next_trigger(pin2.negedge_event());
	} else if (IS_L(pin1)) {
		#define PIN(bit, pin_a, pin_b, pin_out) << pin_a
		TRACE(<< " s " << pin1 << " a " XMUXI16_PINS());
		#undef PIN
		#define PIN(bit, pin_a, pin_b, pin_out) pin_out = AS(IS_L(pin_a));
		XMUXI16_PINS()
		#undef PIN
	} else {
		#define PIN(bit, pin_a, pin_b, pin_out) << pin_b
		TRACE(<< " s " << pin1 << " b " XMUXI16_PINS());
		#undef PIN
		#define PIN(bit, pin_a, pin_b, pin_out) pin_out = AS(IS_L(pin_b));
		XMUXI16_PINS()
		#undef PIN
	}
}
