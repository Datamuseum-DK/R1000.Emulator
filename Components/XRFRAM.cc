#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS
#include "XRFRAM.hh"

static const char *READING = "r";
static const char *WRITING = "w";
static const char *ZZZING = "z";

struct scm_xrfram_state {
	struct ctx ctx;
	uint64_t ram[1024];
	uint64_t last;
	const char *what;
};

SCM_XRFRAM :: SCM_XRFRAM(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << PIN_WE << PIN_CS
	    << PIN_A0 << PIN_A1 << PIN_A2 << PIN_A3 << PIN_A4
	    << PIN_A5 << PIN_A6 << PIN_A7 << PIN_A8 << PIN_A9
#if 0
	    #define PIN(bit, pin_no) << pin_no
	    DQPINS()
	    #undef PIN
#endif
	    ;

	state = (struct scm_xrfram_state *)CTX_Get("XRFRAM", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	state->what = READING;
}

void
SCM_XRFRAM :: doit(void)
{
	unsigned adr = 0;
	uint64_t data = 0;

	state->ctx.activations++;

	if (IS_H(PIN_A9)) adr |= (1 << 0);
	if (IS_H(PIN_A8)) adr |= (1 << 1);
	if (IS_H(PIN_A7)) adr |= (1 << 2);
	if (IS_H(PIN_A6)) adr |= (1 << 3);
	if (IS_H(PIN_A5)) adr |= (1 << 4);
	if (IS_H(PIN_A4)) adr |= (1 << 5);
	if (IS_H(PIN_A3)) adr |= (1 << 6);
	if (IS_H(PIN_A2)) adr |= (1 << 7);
	if (IS_H(PIN_A1)) adr |= (1 << 8);
	if (IS_H(PIN_A0)) adr |= (1 << 9);

	if (IS_H(PIN_CS)) {
		if (state->what == READING) {
			#define PIN(bit, pin_no) pin_no = sc_logic_Z;
			DQPINS()
			#undef PIN
		} else if (state->what == WRITING) {
			#define PIN(bit, pin_no) \
				if (IS_H(pin_no)) data |= ((uint64_t)1 << (63-bit));
			DQPINS()
			#undef PIN
			state->ram[adr] = data;
		}
		next_trigger(PIN_CS.negedge_event());
		state->what = ZZZING;
	} else if (IS_L(PIN_WE)) {
		if (state->what == READING) {
			#define PIN(bit, pin_no) pin_no = sc_logic_Z;
			DQPINS()
			#undef PIN
		}
		#define PIN(bit, pin_no) \
			if (IS_H(pin_no)) data |= ((uint64_t)1 << (63-bit));
		DQPINS()
		#undef PIN
		state->ram[adr] = data;
		state->what = WRITING;
	} else {
		if (state->what == WRITING) {
			#define PIN(bit, pin_no) \
				if (IS_H(pin_no)) data |= ((uint64_t)1 << (63-bit));
			DQPINS()
			#undef PIN
			state->ram[adr] = data;
		}
		data = state->ram[adr];
		if (state->what != READING || data != state->last) {
			#define PIN(bit, pin_no) pin_no = AS(data & ((uint64_t)1 << (63-bit)));
			DQPINS()
			#undef PIN
			state->last = data;
		}
		state->what = READING;
	}

	#define PIN(bit, pin_no) << pin_no
	TRACE(
	    << state->what
	    << " we " << PIN_WE
	    << " cs " << PIN_CS
	    << " a "
	    << PIN_A0 << PIN_A1 << PIN_A2 << PIN_A3 << PIN_A4
	    << PIN_A5 << PIN_A6 << PIN_A7 << PIN_A8 << PIN_A9
	    << " d "
	    DQPINS()
	);
	#undef PIN
}
