#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F194.hh"

// 4-bit bidirectional universal shift register
// Philips 1989 Apr 04, IC15 Data Handbook

struct scm_f194_state {
	struct ctx ctx;
	bool reg[4];
	int job;
};

SCM_F194 :: SCM_F194(sc_module_name nm, const char *arg) : sc_module(nm)
{
	(void)arg;
	SC_METHOD(doit);
	sensitive << pin1 << pin11.pos();

	state = (struct scm_f194_state *)
	    CTX_Get("f194", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	state->reg[0] = true;
	state->reg[1] = true;
	state->reg[2] = true;
	state->reg[3] = true;
	state->job = 1;
}

void
SCM_F194 :: doit(void)
{
	const char *what = NULL;
	bool now[4];

	state->ctx.activations++;
	if (state->job) {
		pin15 = AS(state->reg[0]);
		pin14 = AS(state->reg[1]);
		pin13 = AS(state->reg[2]);
		pin12 = AS(state->reg[3]);
		state->job = 0;
	}
	now[0] = state->reg[0];
	now[1] = state->reg[1];
	now[2] = state->reg[2];
	now[3] = state->reg[3];
	if (IS_L(pin1)) {
		what = " clr ";
		if (state->reg[0] || state->reg[1] || state->reg[2] || state->reg[3]) {
			now[0] = false;
			now[1] = false;
			now[2] = false;
			now[3] = false;
		} else {
			next_trigger(pin1.posedge_event());
		}
	} else if (pin11.posedge()) {
		if (IS_H(pin10) && IS_H(pin9)) {
			what = " load ";
			now[0] = IS_H(pin3);
			now[1] = IS_H(pin4);
			now[2] = IS_H(pin5);
			now[3] = IS_H(pin6);
		} else if (IS_L(pin10) && IS_H(pin9)) {
			what = " right ";
			now[3] = state->reg[2];
			now[2] = state->reg[1];
			now[1] = state->reg[0];
			now[0] = IS_H(pin2);
		} else if (IS_H(pin10) && IS_L(pin9)) {
			what = " left ";
			now[0] = state->reg[1];
			now[1] = state->reg[2];
			now[2] = state->reg[3];
			now[3] = IS_H(pin7);
		}
	}
	if ((state->ctx.do_trace & 2) && what == NULL)
		what = " - ";
	if (what != NULL) {
		TRACE(
		    << what
		    << " mr_ " << pin1 // MR_
		    << " dsr " << pin2 // DSR
		    << " d "
		    << pin3 // D0
		    << pin4 // D1
		    << pin5 // D2
		    << pin6 // D3
		    << " dsl " << pin7 // DSL
		    << " s " << pin9 << pin10
		    << " cp " << pin11 // CP
		    << " r "
		    << state->reg[0] << state->reg[1] << state->reg[2] << state->reg[3]
		);
	}
	if (now[0] != state->reg[0] ||
	    now[1] != state->reg[1] ||
	    now[2] != state->reg[2] ||
	    now[3] != state->reg[3]) {
		state->reg[0] = now[0];
		state->reg[1] = now[1];
		state->reg[2] = now[2];
		state->reg[3] = now[3];
		state->job = 1;
		next_trigger(1, SC_NS);
	}
}
