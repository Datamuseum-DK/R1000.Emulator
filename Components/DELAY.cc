#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "DELAY.hh"

struct scm_delay_state {
	struct ctx ctx;
	unsigned delay;
	int job;
	bool out;
};

SCM_DELAY :: SCM_DELAY(sc_module_name nm, const char *arg) : sc_module(nm)
{
	int i;
	unsigned u;

	SC_METHOD(doit);
	sensitive << pin1;

	state = (struct scm_delay_state *)
	    CTX_Get("delay", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	i = sscanf(arg, "%u ns", &u);
	if (i == 1) {
		state->delay = u;
	} else {
		state->delay = 5;
		cout << "DELAY XXX ARG " << arg << " = " << i << " " << u << "\n";
	}
}

void
SCM_DELAY :: doit(void)
{
	TRACE(
	    << " job " << state->job
	    << " out " << state->out
	    << " " << pin1
	);
	if (state->job) {
		pin2 = AS(state->out);
		state->job = 0;
	}
	if (IS_H(pin1) != state->out) {
		state->job = 1;
		state->out = IS_H(pin1);
		next_trigger(state->delay, SC_NS);
	}
}
