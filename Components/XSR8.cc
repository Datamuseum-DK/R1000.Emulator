#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XSR8.hh"

// 4-bit bidirectional universal shift register
// Philips 1989 Apr 04, IC15 Data Handbook

struct scm_xsr8_state {
	struct ctx ctx;
	unsigned out;
	int job;
};

SCM_XSR8 :: SCM_XSR8(sc_module_name nm, const char *arg) : sc_module(nm)
{
	(void)arg;
	SC_METHOD(doit);
	sensitive << pin2 << pin1.pos();

	state = (struct scm_xsr8_state *)
	    CTX_Get("xsr8", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	state->out = 0xff;
	state->job = 1;
}

void
SCM_XSR8 :: doit(void)
{
	const char *what = NULL;
	unsigned nxt;

	state->ctx.activations++;
	if (state->job) {
		pin15 = AS(state->out & (1<<7));
		pin16 = AS(state->out & (1<<6));
		pin17 = AS(state->out & (1<<5));
		pin18 = AS(state->out & (1<<4));
		pin19 = AS(state->out & (1<<3));
		pin20 = AS(state->out & (1<<2));
		pin21 = AS(state->out & (1<<1));
		pin22 = AS(state->out & (1<<0));
		state->job = 0;
	}
	nxt = state->out;
	if (IS_L(pin2)) {
		what = " clr ";
		if (nxt)
			nxt = 0;
		else
			next_trigger(pin1.posedge_event());
	} else if (pin1.posedge()) {
		if (IS_H(pin5) && IS_H(pin6)) {
			what = " load ";
			nxt = 0;
			if (IS_H(pin7)) nxt |= (1<<7);
			if (IS_H(pin8)) nxt |= (1<<6);
			if (IS_H(pin9)) nxt |= (1<<5);
			if (IS_H(pin10)) nxt |= (1<<4);
			if (IS_H(pin11)) nxt |= (1<<3);
			if (IS_H(pin12)) nxt |= (1<<2);
			if (IS_H(pin13)) nxt |= (1<<1);
			if (IS_H(pin14)) nxt |= (1<<0);
		} else if (IS_L(pin6) && IS_H(pin5)) {
			what = " right ";
			nxt >>= 1;
			if (IS_H(pin3)) nxt |= (1<<7);
		} else if (IS_H(pin6) && IS_L(pin5)) {
			what = " left ";
			nxt <<= 1;
			nxt &= 0xff;
			if (IS_H(pin4)) nxt |= (1<<0);
		}
	}
	if ((state->ctx.do_trace & 2) && what == NULL)
		what = " - ";
	if (what != NULL) {
		TRACE(
		    << what
		    << " mr_ " << pin2 // MR_
		    << " dsr " << pin3 // DSR
		    << " d "
		    << pin7
		    << pin8
		    << pin9
		    << pin10
		    << pin11
		    << pin12
		    << pin13
		    << pin14
		    << " dsl " << pin4 // DSL
		    << " s " << pin5 << pin6
		    << " cp " << pin1 // CP
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
