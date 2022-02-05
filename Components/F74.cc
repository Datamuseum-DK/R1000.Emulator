#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F74.hh"

// (Dual) D-Type Positive Edge-Triggered Flip-Flop
// Fairchild DS009469 April 1988 Revised September 2000

struct scm_f74_state {
	struct ctx ctx;
	bool dreg[2];
	int job;
};

SCM_F74 :: SCM_F74(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin4 << pin3.pos();

	state = (struct scm_f74_state *)
	    CTX_Get("f74", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	state->job = 1;
	state->dreg[0] = true;
	state->dreg[1] = false;
}

void
SCM_F74 :: doit(void)
{
	bool oldreg[2], now;
	const char *what = " ? ";

	if (state->job) {
		pin5 = AS(state->dreg[0]);
		pin6 = AS(state->dreg[1]);
		state->job = 0;
	}
	state->ctx.activations++;
	memcpy(oldreg, state->dreg, sizeof oldreg);
	if (IS_L(pin1) && IS_L(pin4)) {
		if (!state->dreg[0] || !state->dreg[1]) {
			state->dreg[0] = state->dreg[1] = true;
			state->job = 1;
			what = " 2L ";
		}
	} else if (IS_L(pin1)) {
		if (state->dreg[0]) {
			state->dreg[0] = false;
			state->dreg[1] = true;
			state->job = 1;
		}
		what = " CLR ";
	} else if (IS_L(pin4)) {
		if (!state->dreg[0]) {
			state->dreg[0] = true;
			state->dreg[1] = false;
			state->job = 1;
		}
		what = " SET ";
	} else if (pin3.posedge()) {
		now = IS_H(pin2);
		if (state->dreg[0] != now) {
			state->dreg[0] = now;
			state->dreg[1] = !now;
			state->job = 1;
			what = " CHG ";
		} else {
			what = " CLK ";
		}
	}
	if (memcmp(oldreg, state->dreg, sizeof oldreg) || (state->ctx.do_trace & 2)) {
		TRACE(
		    << what
		    << " clr " << pin1
		    << " set " << pin4
		    << " data " << pin2
		    << " clk " << pin3
		    << " job " << state->job
		    << " | "
		    << state->dreg[0]
		    << state->dreg[1]
		);
	}
	if (state->job)
		next_trigger(5, SC_NS);
}
