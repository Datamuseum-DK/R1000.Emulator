#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XUAMUX.hh"

struct scm_xuamux_state {
	struct ctx ctx;
};

SCM_XUAMUX :: SCM_XUAMUX(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) \
	    << pin_a << pin_b << pin_c << pin_de << pin_fgh
	sensitive XUAMUX_PINS() << pin85 << pin86 << pin87 << pin88;
	#undef PIN

	state = (struct scm_xuamux_state *)CTX_Get("XUAMUX", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XUAMUX :: doit(void)
{
	unsigned s = 0;
	state->ctx.activations++;

	if (IS_H(pin88)) {
		TRACE(<< "Z");
		#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) pin_q = sc_logic_Z;
		XUAMUX_PINS()
		#undef PIN
		next_trigger(pin88.negedge_event());
		return;
	}
	if (IS_H(pin85)) s |= 4;
	if (IS_H(pin86)) s |= 2;
	if (IS_H(pin87)) s |= 1;
	switch (s) {
	case 0:
		#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) << pin_a
		TRACE(<< " case " << s << " i " XUAMUX_PINS());
		#undef PIN

		#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) pin_q = AS(IS_L(pin_a));
		XUAMUX_PINS()
		#undef PIN

		break;
	case 1:
		#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) << pin_b
		TRACE(<< " dec " << s << " i " XUAMUX_PINS());
		#undef PIN

		#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) pin_q = AS(IS_L(pin_b));
		XUAMUX_PINS()
		#undef PIN
		break;
	case 2:
		#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) << pin_c
		TRACE(<< " tos " << s << " i " XUAMUX_PINS());
		#undef PIN

		#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) pin_q = AS(IS_L(pin_c));
		XUAMUX_PINS()
		#undef PIN
		break;
	case 3:
	case 4:
		#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) << pin_de
		TRACE(<< " c_uadr " << s << " i " XUAMUX_PINS());
		#undef PIN

		#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) pin_q = AS(IS_L(pin_de));
		XUAMUX_PINS()
		#undef PIN
		break;
	case 5:
	case 6:
	case 7:
		#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) << pin_fgh
		TRACE(<< " brnch " << s << " i " XUAMUX_PINS());
		#undef PIN

		#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) pin_q = AS(IS_L(pin_fgh));
		XUAMUX_PINS()
		#undef PIN
		break;
	}
}
