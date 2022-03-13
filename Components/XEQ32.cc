#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS

#include "XEQ32.hh"

struct scm_xeq32_state {
	struct ctx ctx;
};

SCM_XEQ32 :: SCM_XEQ32(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	#define PINM(pin_a, pin_b) << pin_a << pin_b
	sensitive << PIN_E PIN_PAIRS(PINM);
	#undef PINM

	state = (struct scm_xeq32_state *)CTX_Get("XEQ32", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XEQ32 :: doit(void)
{
	bool retval = true;

	state->ctx.activations++;

	do {
		if (IS_H(PIN_E))
			break;
		#define PINM(pin_a, pin_b) if (IS_L(pin_a) != IS_L(pin_b)) break;
		PIN_PAIRS(PINM)
		#undef PINM
		retval = false;
		break;
	} while(0);
	PIN_AeqB = AS(retval);
}
