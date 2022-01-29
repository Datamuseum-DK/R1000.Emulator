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

	if (strstr(this->name(), "ECNAN4B") != NULL) {
		SC_THREAD(doit_t);
	} else if (strstr(this->name(), "RDNAN0A") != NULL) {
		SC_METHOD(doit_m);
		state->dly = 10;
	} else if (strstr(this->name(), "RDNAN0B") != NULL) {
		SC_METHOD(doit_m);
		state->dly = 10;
	} else {
		SC_METHOD(doit_m);
	}
}

void
SCM_F37 :: doit_t(void)
{
	while (1) {
		pin3 = AS(1);
		TRACE( << pin1 << pin2 << " 1" );
		if (IS_L(pin1)) {
			wait(pin1.posedge_event());
			state->ctx.activations++;
		}
		if (IS_L(pin2)) {
			wait(pin2.posedge_event());
			state->ctx.activations++;
		}
		if (IS_H(pin1) && IS_H(pin2)) {
			wait(state->dly, SC_NS);
			state->ctx.activations++;
			TRACE( << pin1 << pin2 << " 0" );
			pin3 = AS(0);
		}
		if (IS_H(pin1) && IS_H(pin2)) {
			wait(pin1.negedge_event() | pin2.negedge_event());
			state->ctx.activations++;
			wait(state->dly, SC_NS);
		}
	}
}

void
SCM_F37 :: doit_m(void)
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
			next_trigger(state->dly, SC_NS);
		} else {
			next_trigger(pin1.negedge_event() | pin2.negedge_event());
		}
	} else {
		if (!state->out) {
			state->out = true;
			state->task = 1;
			next_trigger(state->dly, SC_NS);
		} else if(IS_L(pin1)) {
			next_trigger(pin1.posedge_event());
		} else {
			next_trigger(pin2.posedge_event());
		}
	}
}
