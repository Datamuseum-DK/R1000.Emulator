#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XREG64.hh"

struct scm_xreg64_state {
	struct ctx ctx;
	bool reg[64];
	int job;
};

SCM_XREG64 :: SCM_XREG64(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1.pos() << pin2;

	state = (struct scm_xreg64_state *)CTX_Get("XREG64", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XREG64 :: doit(void)
{
	bool nxt[64];

	state->ctx.activations++;

	#define PIN(bit,pin_in,pin_out) \
	<< state->reg[bit]
	TRACE(
	    << " job " << state->job
	    << " le " << pin1.posedge()
	    << " oe " << pin2
	    << " | " XREG64_PINS()
	);
	#undef PIN


	if (state->job > 0) {
		#define PIN(bit,pin_in,pin_out) \
		pin_out = AS(state->reg[bit]);
		XREG64_PINS()
		#undef PIN
		state->job = 0;
	} else if (state->job < 0) {
		#define PIN(bit,pin_in,pin_out) \
		pin_out = sc_logic_Z;
		XREG64_PINS()
		#undef PIN
	}

	if (pin1.posedge()) {
		if (IS_H(pin2)) {
			#define PIN(bit,pin_in,pin_out) \
			state->reg[bit] = IS_H(pin_in);
			XREG64_PINS()
			#undef PIN
		} else {

			#define PIN(bit,pin_in,pin_out) \
			nxt[bit] = IS_H(pin_in); \
			if (nxt[bit] != state->reg[bit]) state->job = 1;
			XREG64_PINS()
			#undef PIN

			if (state->job != 0) {
				state->job = 1;
				#define PIN(bit,pin_in,pin_out) \
				state->reg[bit] = nxt[bit];
				XREG64_PINS()
				next_trigger(5, SC_NS);
			}
		}
	}

	if (IS_H(pin2)) {
		if (state->job >= 0) {
			state->job = -1;
			next_trigger(5, SC_NS);
		}
	} else if (state->job != 0) {
		state->job = 1;
		next_trigger(5, SC_NS);
	}
}
