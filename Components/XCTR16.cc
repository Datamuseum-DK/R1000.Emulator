#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XCTR16.hh"

// 4 times:
// 4-Stage Synchronous Bidirectional Counter
// Fairchild DS009488 April 1988 Revised September 2000

struct scm_xctr16_state {
	struct ctx ctx;
	unsigned count;
};

SCM_XCTR16 :: SCM_XCTR16(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin33.pos() << pin37 << pin35;

	state = (struct scm_xctr16_state *)
	    CTX_Get("xctr16", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XCTR16 :: doit(void)
{
	bool carry15;
	bool carry11;
	const char *what = NULL;

	state->ctx.activations++;
	if (pin33.posedge()) {
		if (IS_L(pin34)) {
			what = " ld ";
			state->count = 0;
			#define PIN(bit, pin_in, pin_out) \
			if (IS_H(pin_in)) state->count |= (1<<(15-bit));
			XCTR16_PINS()
			#undef PIN
		} else if (IS_L(pin36) && IS_L(pin37)) {
			if (IS_H(pin35)) {
				what = " up ";
				state->count += 0x0001;
			} else {
				what = " dn ";
				state->count += 0xffff;
			}
			state->count &= 0xffff;
		} else {
			next_trigger(
				pin34.negedge_event() |
				pin36.default_event() |
				pin37.default_event()
			);
		}
	}
	carry11 = true;
	carry15 = true;
	if (IS_L(pin37)) {
		if (IS_H(pin35) && (state->count & 0xfff) == 0xfff)
			carry11 = false;
		if (IS_H(pin35) && state->count == 0xffff)
			carry15 = false;
		if (IS_L(pin35) && state->count == 0x0)
			carry15 = false;
		if (IS_L(pin35) && (state->count & 0xfff) == 0x0)
			carry11 = false;
	}
	if ((state->ctx.do_trace & 2) && what == NULL)
		what = " - ";
	if (what != NULL) {
		#define PIN(bit, pin_in, pin_out) << pin_in
		TRACE(
		    << what
		    << " clk " << pin33
		    << " ud " << pin35
		    << " load " << pin34
		    << " in "
		    XCTR16_PINS()
		    << " enp " << pin36
		    << " ent " << pin37
		    << " | "
		    << std::hex << state->count
		    << " +11 " << carry11
		    << " +15 " << carry15
		);
		#undef PIN
	}
	#define PIN(bit, pin_in, pin_out) \
	pin_out = AS(state->count & (1<<(15-bit)));
	XCTR16_PINS()
	#undef PIN
	pin38 = AS(carry15);
	pin39 = AS(carry11);
}
