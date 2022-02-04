#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F04.hh"

// (Hex) Inverter

struct scm_f04_state {
	struct ctx ctx;
	bool out;
	int task;
};

SCM_F04 :: SCM_F04(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1;

	state = (struct scm_f04_state *)
	    CTX_Get("f04", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	state->task = 2;
}

void
SCM_F04 :: doit(void)
{
	state->ctx.activations++;


	if (state->task == 2)
		state->out = IS_L(pin1);
	TRACE(<< pin1 << " " << state->task << " | " << state->out);
	if (state->task >= 1) {
		pin2 = AS(state->out);
		state->task = 0;
	}
	if (IS_L(pin1) ^ state->out) {
		state->out = IS_L(pin1);
		state->task = 1;
		next_trigger(5, SC_NS);
	}
}
