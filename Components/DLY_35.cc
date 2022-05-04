#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "DLY_35.hh"

struct scm_DLY_35_state {
	struct ctx ctx;
	int next;
	bool state;
};

SCM_DLY_35 :: SCM_DLY_35(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1;

	state = (struct scm_DLY_35_state *)CTX_Get("DLY_35", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_DLY_35 :: doit(void)
{
	state->ctx.activations++;
	if (state->next >= 0) {
		state->state = state->next;
		pin6 = AS(state->state);
		state->next = -1;
	}
	TRACE( << pin1 );
	if (IS_H(pin1) != state->state) {
		state->next = IS_H(pin1);
		next_trigger(28, SC_NS);
	}
}
