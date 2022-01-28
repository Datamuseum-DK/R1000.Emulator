#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F374.hh"

// Octal D-Type Flip-Flop with 3-STATE Outputs
// Fairchild DS009524 May 1988 Revised September 2000

struct scm_f374_state {
	struct ctx ctx;
	bool reg[8];
};

void
SCM_F374 :: loadit(const char *arg)
{
	state = (struct scm_f374_state *)
	    CTX_Get("f374", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	memset(state->reg, 0x00, sizeof state->reg);
}

void
SCM_F374 :: doit(void)
{
	const char *what;
	bool upd;
	bool oreg[8];

	/* TEST_ABUS_PARITY.FIU requires a zero initial state */

	pin2 = AS(state->reg[0]);
	pin5 = AS(state->reg[1]);
	pin6 = AS(state->reg[2]);
	pin9 = AS(state->reg[3]);
	pin12 = AS(state->reg[4]);
	pin15 = AS(state->reg[5]);
	pin16 = AS(state->reg[6]);
	pin19 = AS(state->reg[7]);
	wait(1, SC_NS);
	while(1) {
		wait();
		state->ctx.activations++;
		upd = false;
		what = NULL;

		if (pin11.posedge()) {
			state->reg[0] = IS_H(pin3);
			state->reg[1] = IS_H(pin4);
			state->reg[2] = IS_H(pin7);
			state->reg[3] = IS_H(pin8);
			state->reg[4] = IS_H(pin13);
			state->reg[5] = IS_H(pin14);
			state->reg[6] = IS_H(pin17);
			state->reg[7] = IS_H(pin18);
			upd = true;
			if (memcmp(state->reg, oreg, sizeof oreg)) {
				what = "new ";
				memcpy(oreg, state->reg, sizeof oreg);
			} else if (state->ctx.do_trace & 2) {
				what = "new ";
			}
		}
		if (IS_H(pin1)) {
			if ((state->ctx.do_trace & 2) && what == NULL)
				what = "Z ";
			pin2 = sc_logic_Z;
			pin5 = sc_logic_Z;
			pin6 = sc_logic_Z;
			pin9 = sc_logic_Z;
			pin12 = sc_logic_Z;
			pin15 = sc_logic_Z;
			pin16 = sc_logic_Z;
			pin19 = sc_logic_Z;
		} else {
			if ((state->ctx.do_trace & 2) && what == NULL)
				what = "out ";
			if (upd)
				wait(1, SC_NS);
			pin2 = AS(state->reg[0]);
			pin5 = AS(state->reg[1]);
			pin6 = AS(state->reg[2]);
			pin9 = AS(state->reg[3]);
			pin12 = AS(state->reg[4]);
			pin15 = AS(state->reg[5]);
			pin16 = AS(state->reg[6]);
			pin19 = AS(state->reg[7]);
		}
		if (what != NULL) {
			TRACE(
			    << what
			    << " OE_ " << pin1
			    << " CK " << pin11
			    << " D "
			    << pin3 << pin4 << pin7 << pin8
			    << pin13 << pin14 << pin17 << pin18
			    << "|"
			    << state->reg[0]
			    << state->reg[1]
			    << state->reg[2]
			    << state->reg[3]
			    << state->reg[4]
			    << state->reg[5]
			    << state->reg[6]
			    << state->reg[7]
			);
		}
	}
}
