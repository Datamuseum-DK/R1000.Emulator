// 6 bit two input multiplexer

#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS
#include "XMUX6.hh"

struct scm_xmux6_state {
	struct ctx ctx;
};

SCM_XMUX6 :: SCM_XMUX6(sc_module_name nm, const char *arg) : sc_module(nm)
{

	state = (struct scm_xmux6_state *)CTX_Get("XMUX6", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);

	SC_METHOD(doit);
	#define PINM(pin_a, pin_b, pin_y) << pin_a << pin_b
	sensitive << PIN_S << PIN_E PIN_SETS(PINM);
	#undef PINM
}

void
SCM_XMUX6 :: doit(void)
{
	bool inv = IS_L(PIN_INV);

	state->ctx.activations++;

	if (IS_H(PIN_E)) {
		TRACE( << " e ");
		#define PINM(pin_a, pin_b, pin_y) pin_y = AS(inv);
		PIN_SETS(PINM)
		#undef PINM
		next_trigger(PIN_E.negedge_event());
	} else if (IS_L(PIN_S)) {
		#define PINM(pin_a, pin_b, pin_y) pin_y = AS(IS_H(pin_a) ^ inv);
		PIN_SETS(PINM)
		#undef PINM
	} else {
		#define PINM(pin_a, pin_b, pin_y) pin_y = AS(IS_H(pin_b) ^ inv);
		PIN_SETS(PINM)
		#undef PINM
	}
}
