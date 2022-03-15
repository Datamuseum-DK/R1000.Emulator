// 8 bit adder

#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS
#include "XADD8.hh"

struct scm_xadd8_state {
	struct ctx ctx;
};

SCM_XADD8 :: SCM_XADD8(sc_module_name nm, const char *arg) : sc_module(nm)
{

	state = (struct scm_xadd8_state *)CTX_Get("XADD8", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);

	SC_METHOD(doit);
	#define PINM(bit, pin) << pin
	sensitive PIN_SETS_IN(PINM);
	#undef PINM
}

void
SCM_XADD8 :: doit(void)
{
	uint64_t sum = 0;

	state->ctx.activations++;

	#define PINM(bit, pin) if (IS_H(pin)) sum += (1ULL << bit);
	PIN_SETS_IN(PINM);
	#undef PINM

	#define PINM(bit, pin) pin = AS(sum & (1ULL << bit));
	PIN_SETS_OUT(PINM);
	#undef PINM
}
