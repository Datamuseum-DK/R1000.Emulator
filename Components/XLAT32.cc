#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XLAT32.hh"

struct scm_xlat32_state {
	struct ctx ctx;
	bool reg[32];
	int job;
};

SCM_XLAT32 :: SCM_XLAT32(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	#define PIN(bit, pin_in, pin_out) << pin_in
	sensitive << pin1.pos() << pin2 XLAT32_PINS();
	#undef PIN

	state = (struct scm_xlat32_state *)CTX_Get("XLAT32", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XLAT32 :: doit(void)
{
	bool nxt[32];

	state->ctx.activations++;

	#define PIN(bit,pin_in,pin_out) \
	<< state->reg[bit]
	TRACE(
	    << " job " << state->job
	    << " le " << pin1.posedge()
	    << " oe " << pin2
	    << " | " XLAT32_PINS()
	);
	#undef PIN


	if (state->job > 0) {
		#define PIN(bit,pin_in,pin_out) \
		pin_out = AS(state->reg[bit]);
		XLAT32_PINS()
		#undef PIN
		state->job = 0;
	} else if (state->job < 0) {
		#define PIN(bit,pin_in,pin_out) \
		pin_out = sc_logic_Z;
		XLAT32_PINS()
		#undef PIN
	}

	if (IS_H(pin1)) {
		#define PIN(bit,pin_in,pin_out) \
		nxt[bit] = IS_H(pin_in); \
		if (nxt[bit] != state->reg[bit]) state->job = 1;
		XLAT32_PINS()
		memcpy(state->reg, nxt, sizeof state->reg);
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
