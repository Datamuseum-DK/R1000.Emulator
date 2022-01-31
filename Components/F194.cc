#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F194.hh"

// 4-bit bidirectional universal shift register
// Philips 1989 Apr 04, IC15 Data Handbook

struct scm_f194_state {
	struct ctx ctx;
	bool reg[4];
};

SCM_F194 :: SCM_F194(sc_module_name nm, const char *arg) : sc_module(nm)
{
	(void)arg;
	SC_THREAD(doit);
	sensitive << pin1 << pin11.pos();

	state = (struct scm_f194_state *)
	    CTX_Get("f194", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F194 :: doit(void)
{

	while (1) {
		wait();
		const char *what = NULL;
		state->ctx.activations++;
		if (!IS_H(pin1)) {
			what = " clr ";
			state->reg[0] = false;
			state->reg[1] = false;
			state->reg[2] = false;
			state->reg[3] = false;
		} else if (pin11.posedge()) {
			if (IS_H(pin10) && IS_H(pin9)) {
				what = " load ";
				state->reg[0] = !IS_L(pin3);
				state->reg[1] = !IS_L(pin4);
				state->reg[2] = !IS_L(pin5);
				state->reg[3] = !IS_L(pin6);
			} else if (IS_L(pin10) && IS_H(pin9)) {
				what = " right ";
				state->reg[3] = state->reg[2];
				state->reg[2] = state->reg[1];
				state->reg[1] = state->reg[0];
				state->reg[0] = IS_H(pin2);
			} else if (IS_H(pin10) && IS_L(pin9)) {
				what = " left ";
				state->reg[0] = state->reg[1];
				state->reg[1] = state->reg[2];
				state->reg[2] = state->reg[3];
				state->reg[3] = IS_H(pin7);
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
			wait(1, SC_NS);
			pin15 = AS(state->reg[0]);
			pin14 = AS(state->reg[1]);
			pin13 = AS(state->reg[2]);
			pin12 = AS(state->reg[3]);
		}
	}
}
