#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F194.hh"

// 4-bit bidirectional universal shift register
// Philips 1989 Apr 04, IC15 Data Handbook

struct scm_f194_state {
	struct ctx ctx;
	unsigned out;
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
	state->out = 0xf;
	state->job = 1;
}

void
SCM_F194 :: doit(void)
{
	const char *what = NULL;
	unsigned nxt;

	state->ctx.activations++;
	if (state->job) {
		pin15 = AS(state->out & (1<<3));
		pin14 = AS(state->out & (1<<2));
		pin13 = AS(state->out & (1<<1));
		pin12 = AS(state->out & (1<<0));
		state->job = 0;
	}
	nxt = state->out;
	if (IS_L(pin1)) {
		what = " clr ";
		if (nxt)
			nxt = 0;
		else
			next_trigger(pin1.posedge_event());
	} else if (pin11.posedge()) {
		if (IS_H(pin10) && IS_H(pin9)) {
			what = " load ";
			nxt = 0;
			if (IS_H(pin3)) nxt |= (1<<3);
			if (IS_H(pin4)) nxt |= (1<<2);
			if (IS_H(pin5)) nxt |= (1<<1);
			if (IS_H(pin6)) nxt |= (1<<0);
		} else if (IS_L(pin10) && IS_H(pin9)) {
			what = " right ";
			nxt >>= 1;
			if (IS_H(pin2)) nxt |= (1<<3);
		} else if (IS_H(pin10) && IS_L(pin9)) {
			what = " left ";
			nxt <<= 1;
			nxt &= 0xf;
			if (IS_H(pin7)) nxt |= (1<<0);
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
		    << std::hex << state->out
		    << " nxt "
		    << std::hex << nxt
		);
	}
	if (nxt != state->out) {
		state->out = nxt;
		state->job = 1;
		next_trigger(1, SC_NS);
	}
}
