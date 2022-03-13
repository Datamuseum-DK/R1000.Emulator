#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS

#include "XEQ20.hh"

struct scm_xeq20_state {
	struct ctx ctx;
};

SCM_XEQ20 :: SCM_XEQ20(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	#define PINM(pin_a, pin_b) << pin_a << pin_b
	sensitive << PIN_E PIN_PAIRS(PINM);
	#undef PINM

	state = (struct scm_xeq20_state *)CTX_Get("XEQ20", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XEQ20 :: doit(void)
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
