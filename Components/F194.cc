#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS
#include "F194.hh"

// 4-bit bidirectional universal shift register
// Philips 1989 Apr 04, IC15 Data Handbook

struct scm_f194_state {
	struct ctx ctx;
	unsigned out;
	int job;
};

SCM_F194 :: SCM_F194(sc_module_name nm, const char *arg) : sc_module(nm)
{
	(void)arg;
	SC_METHOD(doit);
	sensitive << PIN_CLR << PIN_CLK.pos();

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
	unsigned nxt, mode = 0;

	state->ctx.activations++;
	if (state->job) {
		PIN_Q0 = AS(state->out & (1<<3));
		PIN_Q1 = AS(state->out & (1<<2));
		PIN_Q2 = AS(state->out & (1<<1));
		PIN_Q3 = AS(state->out & (1<<0));
		state->job = 0;
	}
	nxt = state->out;
	if (IS_L(PIN_CLR)) {
		what = " clr ";
		if (nxt)
			nxt = 0;
		else
			next_trigger(PIN_CLR.posedge_event());
	} else if (PIN_CLK.posedge()) {
		if (IS_H(PIN_S0)) mode |= 2;
		if (IS_H(PIN_S1)) mode |= 1;
		switch (mode) {
		case 3:
			what = " load ";
			nxt = 0;
			if (IS_H(PIN_D0)) nxt |= (1<<3);
			if (IS_H(PIN_D1)) nxt |= (1<<2);
			if (IS_H(PIN_D2)) nxt |= (1<<1);
			if (IS_H(PIN_D3)) nxt |= (1<<0);
			break;
		case 2:
			what = " >> ";
			nxt >>= 1;
			if (IS_H(PIN_RSI)) nxt |= (1<<3);
			break;
		case 1:
			what = " <<ft ";
			nxt <<= 1;
			nxt &= 0xf;
			if (IS_H(PIN_LSI)) nxt |= (1<<0);
			break;
		case 0:
			next_trigger(
			    PIN_CLR.negedge_event() |
			    PIN_S0.posedge_event() |
			    PIN_S1.posedge_event()
			);
		}
	}
	if ((state->ctx.do_trace & 2) && what == NULL)
		what = " - ";
	if (what != NULL) {
		TRACE(
		    << what
		    << " mr_ " << PIN_CLR
		    << " rsi " << PIN_RSI
		    << " d "
		    << PIN_D0
		    << PIN_D1
		    << PIN_D2
		    << PIN_D3
		    << " lsi " << PIN_LSI
		    << " s " << PIN_S0 << PIN_S1
		    << " cp " << PIN_CLK
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
