#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F169.hh"

// 4-Stage Synchronous Bidirectional Counter
// Fairchild DS009488 April 1988 Revised September 2000

struct scm_f169_state {
	struct ctx ctx;
	unsigned count;
};

SCM_F169 :: SCM_F169(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin2.pos() << pin10 << pin1;

	state = (struct scm_f169_state *)
	    CTX_Get("f169", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F169 :: doit(void)
{
	bool carry;
	const char *what = NULL;

	state->ctx.activations++;
	if (pin2.posedge()) {
		if (IS_L(pin9)) {
			what = " ld ";
			state->count = 0;
			if (IS_H(pin3)) state->count |= 1;
			if (IS_H(pin4)) state->count |= 2;
			if (IS_H(pin5)) state->count |= 4;
			if (IS_H(pin6)) state->count |= 8;
		} else if (IS_L(pin7) && IS_L(pin10)) {
			if (IS_H(pin1)) {
				what = " up ";
				state->count = (state->count + 0x1) & 0xf;
			} else {
				what = " dn ";
				state->count = (state->count + 0xf) & 0xf;
			}
		}
	}
	if (IS_H(pin10))
		carry = true;
	else if (IS_H(pin1) && state->count == 0xf)
		carry = false;
	else if (IS_L(pin1) && state->count == 0x0)
		carry = false;
	else
		carry = true;
	if ((state->ctx.do_trace & 2) && what == NULL)
		what = " - ";
	if (what != NULL) {
		TRACE(
		    << what
		    << " ud " << pin1
		    << " clk " << pin2
		    << " abcd " << pin6 << pin5 << pin4 << pin3
		    << " enp " << pin7
		    << " load " << pin9
		    << " ent " << pin10
		    << "|"
		    << std::hex << state->count
		    << "+"
		    << carry
		);
	}
	pin14 = AS(state->count & 1);
	pin13 = AS(state->count & 2);
	pin12 = AS(state->count & 4);
	pin11 = AS(state->count & 8);
	pin15 = AS(carry);
}
