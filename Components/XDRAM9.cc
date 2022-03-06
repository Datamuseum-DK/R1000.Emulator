#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XDRAM9.hh"

struct scm_xdram9_state {
	struct ctx ctx;
	uint16_t ram[1<<20];
	unsigned ras, cas;
};

SCM_XDRAM9 :: SCM_XDRAM9(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2 << pin22;

	state = (struct scm_xdram9_state *)CTX_Get("XDRAM9", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XDRAM9 :: doit(void)
{
	unsigned adr = 0;
	uint16_t data = 0;

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

if (0) {
	if (pin22.negedge())
		state->ras = adr;

	if (pin2.negedge()) {
		state->cas = adr;
		adr = (state->cas << 10) | state->ras;

		if (IS_L(pin1)) {
			#define PIN(bit, pin_no) pin_no = sc_logic_Z;
			DQPINS9()
			#undef PIN
		} else {
			data = state->ram[adr];
			#define PIN(bit, pin_no) pin_no = AS(data & ((uint64_t)1 << bit));
			DQPINS9()
			#undef PIN
		}
	}
	if (pin2.posedge()) {
		#define PIN(bit, pin_no) pin_no = sc_logic_Z;
		DQPINS9()
		#undef PIN
	}


	if (IS_L(pin1) && IS_L(pin2)) {
		#define PIN(bit, pin_no) \
			if (IS_H(pin_no)) data |= ((uint64_t)1 << bit);
		DQPINS9()
		#undef PIN
		state->ram[adr] = data;
	    #define PIN(bit, pin_no) << pin_no
	TRACE(
	    << " we " <<pin1
	    << " cs " <<pin2
	    << " a "
	    <<pin3 <<pin4 <<pin5 <<pin6 <<pin7 <<pin8
	    <<pin9 <<pin10 <<pin11 <<pin12
	    << " d "
	    DQPINS9()
	);
	    #undef PIN
	}
	if (IS_L(pin2) && IS_H(pin1)) {
		data = state->ram[adr];
		#define PIN(bit, pin_no) pin_no = AS(data & ((uint64_t)1 << bit));
		DQPINS9()
		#undef PIN
	}
}
}
