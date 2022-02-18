#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XMUX16.hh"

struct scm_xmux16_state {
	struct ctx ctx;
};

SCM_XMUX16 :: SCM_XMUX16(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	#define PIN(bit, pin_a, pin_b, pin_out) << pin_a << pin_b
	sensitive << pin1 << pin2 XMUX16_PINS();
	#undef PIN

	state = (struct scm_xmux16_state *)CTX_Get("XMUX16", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XMUX16 :: doit(void)
{
	state->ctx.activations++;

	if (IS_H(pin2)) {
		TRACE( << " Z ");
		#define PIN(bit, pin_a, pin_b, pin_out) pin_out = sc_logic_Z;
		XMUX16_PINS()
		#undef PIN
		next_trigger(pin2.negedge_event());
	} else if (IS_L(pin1)) {
		#define PIN(bit, pin_a, pin_b, pin_out) << pin_a 
		TRACE(<< " s " << pin1 << " a " XMUX16_PINS());
		#undef PIN
		#define PIN(bit, pin_a, pin_b, pin_out) pin_out = AS(IS_H(pin_a));
		XMUX16_PINS()
		#undef PIN
	} else {
		#define PIN(bit, pin_a, pin_b, pin_out) << pin_b 
		TRACE(<< " s " << pin1 << " b " XMUX16_PINS());
		#undef PIN
		#define PIN(bit, pin_a, pin_b, pin_out) pin_out = AS(IS_H(pin_b));
		XMUX16_PINS()
		#undef PIN
	}
}
