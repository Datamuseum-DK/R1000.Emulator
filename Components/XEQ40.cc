#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS

#include "XEQ40.hh"

struct scm_xeq40_state {
	struct ctx ctx;
};

SCM_XEQ40 :: SCM_XEQ40(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	#define PINM(pin_a, pin_b) << pin_a << pin_b
	sensitive << PIN_E PIN_PAIRS(PINM);
	#undef PINM

	state = (struct scm_xeq40_state *)CTX_Get("XEQ40", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XEQ40 :: doit(void)
{
	bool retval = true;

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
