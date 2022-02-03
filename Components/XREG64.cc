#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XREG64.hh"

struct scm_XREG64_state {
	struct ctx ctx;
	bool reg[64];
};

SCM_XREG64 :: SCM_XREG64(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_THREAD(doit);
	sensitive << pin1.pos() << pin2;

	state = (struct scm_XREG64_state *)CTX_Get("XREG64", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XREG64 :: doit(void)
{

	while(1) {
		wait();
		state->ctx.activations++;

		if (pin1.posedge()) {

			#define PIN(bit,pin_in,pin_out) \
			state->reg[bit] = IS_H(pin_in);
			XREG64_PINS()
			#undef PIN

			#define PIN(bit,pin_in,pin_out) \
			<< state->reg[bit]
			TRACE(<< " new " XREG64_PINS());
			#undef PIN
		}

		if (IS_H(pin2)) {
			TRACE( << " Z " << pin2 << " ");

			#define PIN(bit,pin_in,pin_out) \
			pin_out = sc_logic_Z;
			XREG64_PINS()
			#undef PIN

		} else {

			#define PIN(bit,pin_in,pin_out) \
			<< state->reg[bit]
			TRACE( << " out " << pin2 << " " XREG64_PINS());
			#undef PIN

			#define PIN(bit,pin_in,pin_out) \
			pin_out = AS(state->reg[bit]);
			XREG64_PINS()
			#undef PIN

		}
	}
}
