#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F240.hh"

// Octal inverting buffers (3-state)
// Philips 2004 Feb 25

struct scm_f240_state {
	struct ctx ctx;
	int job1, job2;
	bool out1[4], out2[4];
};

SCM_F240 :: SCM_F240(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit1);
	sensitive << pin1 << pin2 << pin4 << pin6 << pin8;
	SC_METHOD(doit2);
	sensitive << pin19 << pin11 << pin13 << pin15 << pin17;

	state = (struct scm_f240_state *)
	    CTX_Get("f240", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	state->job1 = -1;
	state->job2 = -1;
}

void
SCM_F240 :: doit1(void)
{
	bool next[4];
	const char *nxt;

	state->ctx.activations++;
	if (state->job1 > 0) {
		pin18 = AS(state->out1[0]);
		pin16 = AS(state->out1[1]);
		pin14 = AS(state->out1[2]);
		pin12 = AS(state->out1[3]);
		state->job1 = 0;
	} else if (state->job1 < 0) {
		pin18 = sc_logic_Z;
		pin16 = sc_logic_Z;
		pin14 = sc_logic_Z;
		pin12 = sc_logic_Z;
	}

	if (IS_H(pin1) && state->job1 >= 0) {
		state->job1 = -1;
		next_trigger(5, SC_NS);
		nxt = " ^Z ";
	} else if (IS_H(pin1)) {
		next_trigger(pin1.negedge_event());
		nxt = " =Z ";
	} else {
		next[0] = IS_L(pin2);
		next[1] = IS_L(pin4);
		next[2] = IS_L(pin6);
		next[3] = IS_L(pin8);
		if (state->job1 < 0 ||
		    next[0] != state->out1[0] ||
		    next[1] != state->out1[1] ||
		    next[2] != state->out1[2] ||
		    next[3] != state->out1[3]) {
			state->out1[0] = next[0];
			state->out1[1] = next[1];
			state->out1[2] = next[2];
			state->out1[3] = next[3];
			state->job1 = 1;
			next_trigger(5, SC_NS);
			nxt = " != ";
		} else {
			nxt = " ? ";
		}
	}
	TRACE(
	    << nxt
	    << " OE0_ " << pin1
	    << " D "
	    << pin2 << pin4 << pin6 << pin8
	    << " j " << state->job1
	);
}

void
SCM_F240 :: doit2(void)
{
	bool next[4];
	const char *nxt;

	state->ctx.activations++;
	if (state->job2 > 0) {
		pin3 = AS(state->out2[0]);
		pin5 = AS(state->out2[1]);
		pin7 = AS(state->out2[2]);
		pin9 = AS(state->out2[3]);
		state->job2 = 0;
	} else if (state->job2 < 0) {
		pin3 = sc_logic_Z;
		pin5 = sc_logic_Z;
		pin7 = sc_logic_Z;
		pin9 = sc_logic_Z;
	}

	if (IS_H(pin19) && state->job2 >= 0) {
		state->job2 = -1;
		next_trigger(5, SC_NS);
		nxt = " ^Z ";
	} else if (IS_H(pin19)) {
		next_trigger(pin19.negedge_event());
		nxt = " =Z ";
	} else {
		next[0] = IS_L(pin17);
		next[1] = IS_L(pin15);
		next[2] = IS_L(pin13);
		next[3] = IS_L(pin11);
		if (state->job2 < 0 ||
		    next[0] != state->out2[0] ||
		    next[1] != state->out2[1] ||
		    next[2] != state->out2[2] ||
		    next[3] != state->out2[3]) {
			state->out2[0] = next[0];
			state->out2[1] = next[1];
			state->out2[2] = next[2];
			state->out2[3] = next[3];
			state->job2 = 1;
			next_trigger(5, SC_NS);
			nxt = " != ";
		} else {
			nxt = " ? ";
		}
	}
	TRACE(
	    << nxt
	    << " OE1_ " << pin19
	    << " D "
	    << pin17 << pin15 << pin13 << pin11
	    << " j " << state->job2
	);
}
