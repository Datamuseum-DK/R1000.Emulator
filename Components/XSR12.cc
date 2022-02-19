#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XSR12.hh"

// tripple 4-bit bidirectional universal shift register
// Philips 1989 Apr 04, IC15 Data Handbook

struct scm_xsr12_state {
	struct ctx ctx;
	unsigned out;
	int job;
};

SCM_XSR12 :: SCM_XSR12(sc_module_name nm, const char *arg) : sc_module(nm)
{
	(void)arg;
	SC_METHOD(doit);
	sensitive << pin2 << pin1.pos();

	state = (struct scm_xsr12_state *)
	    CTX_Get("xsr12", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	state->out = 0xfff;
	state->job = 1;
}

void
SCM_XSR12 :: doit(void)
{
	const char *what = NULL;
	unsigned nxt;

	state->ctx.activations++;
	if (state->job) {
		pin19 = AS(state->out & (1<<11));
		pin20 = AS(state->out & (1<<10));
		pin21 = AS(state->out & (1<<9));
		pin22 = AS(state->out & (1<<8));
		pin23 = AS(state->out & (1<<7));
		pin24 = AS(state->out & (1<<6));
		pin25 = AS(state->out & (1<<5));
		pin26 = AS(state->out & (1<<4));
		pin27 = AS(state->out & (1<<3));
		pin28 = AS(state->out & (1<<2));
		pin29 = AS(state->out & (1<<1));
		pin30 = AS(state->out & (1<<0));
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
			if (IS_H(pin7)) nxt |= (1<<11);
			if (IS_H(pin8)) nxt |= (1<<10);
			if (IS_H(pin9)) nxt |= (1<<9);
			if (IS_H(pin10)) nxt |= (1<<8);
			if (IS_H(pin11)) nxt |= (1<<7);
			if (IS_H(pin12)) nxt |= (1<<6);
			if (IS_H(pin13)) nxt |= (1<<5);
			if (IS_H(pin14)) nxt |= (1<<4);
			if (IS_H(pin15)) nxt |= (1<<3);
			if (IS_H(pin16)) nxt |= (1<<2);
			if (IS_H(pin17)) nxt |= (1<<1);
			if (IS_H(pin18)) nxt |= (1<<0);
		} else if (IS_L(pin6) && IS_H(pin5)) {
			what = " right ";
			nxt >>= 1;
			if (IS_H(pin3)) nxt |= (1<<11);
		} else if (IS_H(pin6) && IS_L(pin5)) {
			what = " left ";
			nxt <<= 1;
			nxt &= 0xfff;
			if (IS_H(pin4)) nxt |= (1<<0);
		} else {
			next_trigger(
			    pin1.negedge_event() |
			    pin10.posedge_event() |
			    pin9.posedge_event()
			);
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
		    << pin15
		    << pin16
		    << pin17
		    << pin18
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
		next_trigger(5, SC_NS);
	}
}
