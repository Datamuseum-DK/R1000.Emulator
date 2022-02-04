#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F37.hh"

// (Quad) Two-Input NAND Buffer

struct scm_f37_state {
	struct ctx ctx;
	unsigned dly;
	bool out;
	int task;
};

SCM_F37 :: SCM_F37(sc_module_name nm, const char *arg) : sc_module(nm)
{

	state = (struct scm_f37_state *)
	    CTX_Get("f37", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	state->task = 2;
	state->dly = 5;

	if (strstr(this->name(), "IOC.ioc_54.RDNAN0A") != NULL) {
		// TEST_MACRO_EVENT_SLICE.IOC @ optimized
		state->dly = 10;
	}
	if (strstr(this->name(), "IOC.ioc_54.RDNAN0B") != NULL) {
		// TEST_MACRO_EVENT_DELAY.IOC @ optimized
		state->dly = 10;
	}

	SC_METHOD(doit);
}

void
SCM_F37 :: doit(void)
{

	state->ctx.activations++;

	TRACE( << pin1 << pin2 << " " << state->task << " " << state->out);
	if (state->task == 2) {
		state->out = IS_L(pin1) || IS_L(pin2);
		state->task = 1;
	}
	if (state->task == 1) {
		pin3 = AS(state->out);
		state->task = 0;
	}
	if (IS_H(pin1) && IS_H(pin2)) {
		if (state->out) {
			state->out = false;
			state->task = 1;
			if (state->ctx.do_trace & 2)
				TRACE( << "dv");
			next_trigger(state->dly, SC_NS);
		} else {
			if (state->ctx.do_trace & 2)
				TRACE( << "wv");
			next_trigger(pin1.negedge_event() | pin2.negedge_event());
		}
	} else {
		if (!state->out) {
			state->out = true;
			state->task = 1;
			if (state->ctx.do_trace & 2)
				TRACE( << "d^");
			next_trigger(state->dly, SC_NS);
		} else if (pin1 != sc_logic_1) {
			if (state->ctx.do_trace & 2)
				TRACE( << "w^1");
			next_trigger(pin1.posedge_event());
		} else if (pin2 != sc_logic_1) {
			if (state->ctx.do_trace & 2)
				TRACE( << "w^2");
			next_trigger(pin2.posedge_event());
		} else {
			if (state->ctx.do_trace & 2)
				TRACE( << "w^");
			next_trigger(pin2.posedge_event() | pin1.posedge_event());
		}
	}
}
