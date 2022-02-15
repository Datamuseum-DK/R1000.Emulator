#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XZCNT.hh"

struct scm_xzcnt_state {
	struct ctx ctx;
	unsigned bitno;
};

SCM_XZCNT :: SCM_XZCNT(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1.pos() << pin2;

	state = (struct scm_xzcnt_state *)CTX_Get("XZCNT", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XZCNT :: doit(void)
{

	state->ctx.activations++;


	if (pin1.posedge()) {
		state->bitno = 0x7f;
		do {
			#define PIN(bit, pin_in) \
			if (IS_L(pin_in)) { \
				state->bitno = 63 - bit; \
				break; \
			}
			XZCNT_I_PINS()
			#undef PIN
		} while (0);
		state->bitno ^= 0xffc0;
		#define PIN(bit, pin_in) \
		<< pin_in
		TRACE(
		    << " le " << pin1.posedge()
		    << " oe " << pin2
		    << " " XZCNT_I_PINS()
		    << " | " << std::hex << state->bitno
		);
		#undef PIN
	}
	if (IS_L(pin2)) {
		#define PIN(bit, pin_out) pin_out = AS(state->bitno & (1<<(15-bit)));
		XZCNT_O_PINS()
		#undef PIN
	} else {
		#define PIN(bit, pin_out) pin_out = sc_logic_Z;
		XZCNT_O_PINS()
		#undef PIN
	}
}
