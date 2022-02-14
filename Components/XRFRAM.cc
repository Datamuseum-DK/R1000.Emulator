#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XRFRAM.hh"

#define pinWE pin1
#define pinCS pin2

struct scm_xrfram_state {
	struct ctx ctx;
	uint64_t ram[1024];
};

SCM_XRFRAM :: SCM_XRFRAM(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pinWE << pinCS << pin3 << pin4 << pin5 << pin6
	    << pin7 << pin8 << pin9 << pin10 << pin11 << pin12
	    #define PIN(bit, pin_no) << pin_no
	    DQPINS()
	    #undef PIN
	    ;

	state = (struct scm_xrfram_state *)CTX_Get("XRFRAM", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XRFRAM :: doit(void)
{
	unsigned adr = 0;
	uint64_t data = 0;

	state->ctx.activations++;
	if (IS_H(pin3)) adr |= (1 << 0);
	if (IS_H(pin4)) adr |= (1 << 1);
	if (IS_H(pin5)) adr |= (1 << 2);
	if (IS_H(pin6)) adr |= (1 << 3);
	if (IS_H(pin7)) adr |= (1 << 4);
	if (IS_H(pin8)) adr |= (1 << 5);
	if (IS_H(pin9)) adr |= (1 << 6);
	if (IS_H(pin10)) adr |= (1 << 7);
	if (IS_H(pin11)) adr |= (1 << 8);
	if (IS_H(pin12)) adr |= (1 << 9);
	if (IS_H(pinCS)) {
		#define PIN(bit, pin_no) pin_no = sc_logic_Z;
		DQPINS()
		#undef PIN
		next_trigger(pinCS.negedge_event());
	} else if (IS_L(pinWE)) {
		#define PIN(bit, pin_no) pin_no = sc_logic_Z;
		DQPINS()
		#undef PIN
	}
	if (IS_L(pinWE) && IS_L(pinCS)) {
		#define PIN(bit, pin_no) \
			if (IS_H(pin_no)) data |= ((uint64_t)1 << bit);
		DQPINS()
		#undef PIN
		state->ram[adr] = data;
	}

	#define PIN(bit, pin_no) << pin_no
	TRACE(
	    << " we " <<pinWE
	    << " cs " <<pinCS
	    << " a "
	    <<pin3 <<pin4 <<pin5 <<pin6 <<pin7 <<pin8
	    <<pin9 <<pin10 <<pin11 <<pin12
	    << " d "
	    DQPINS()
	);
	#undef PIN

	if (IS_L(pinCS) && IS_H(pinWE)) {
		data = state->ram[adr];
		#define PIN(bit, pin_no) pin_no = AS(data & ((uint64_t)1 << bit));
		DQPINS()
		#undef PIN
	}
}
