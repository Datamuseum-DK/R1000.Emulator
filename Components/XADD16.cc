#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XADD16.hh"

struct scm_xadd16_state {
	struct ctx ctx;
};

SCM_XADD16 :: SCM_XADD16(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	#define PIN(bit, pin_a, pin_b, pin_out) << pin_a << pin_b
	sensitive << pin1 XADD16_PINS() << pin51;
	#undef PIN

	state = (struct scm_xadd16_state *)CTX_Get("XADD16", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XADD16 :: doit(void)
{
	unsigned a = 0, b = 0, out;

	state->ctx.activations++;

	#define PIN(bit, pin_a, pin_b, pin_out) \
	if (IS_H(pin_a)) a |= (1 << (15 - bit)); \
	if (IS_H(pin_b)) b |= (1 << (15 - bit));
	XADD16_PINS()
	#undef PIN

	if (IS_H(pin51)) {
		out = a + b;
		if (IS_H(pin1))
			out++;
	} else {
		out = (1<<16) + b - a;
		if (IS_L(pin1))
			out--;
		//out ^= (1 << 16);
	}

	#define PIN(bit, pin_a, pin_b, pin_out) \
	pin_out = AS(out & (1 << (15 - bit)));
	XADD16_PINS()
	#undef PIN

	pin2 = AS(out >> 16);

	TRACE(
	    << " add " << pin51
	    << " a " << std::hex << a
	    << " b " << std::hex << b
	    << " ci " << pin1
	    << " | "
	    << std::hex << out <<
	    "+" << (out >> 16)
	);
}
