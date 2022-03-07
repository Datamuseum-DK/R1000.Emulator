#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XPAR64.hh"

struct scm_xpar64_state {
	struct ctx ctx;
};

SCM_XPAR64 :: SCM_XPAR64(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);

	#define PIN(bit, pin_in) << pin_in
	sensitive << pin1 XPAR64_PINS();
	#undef PIN

	state = (struct scm_xpar64_state *)CTX_Get("XPAR64", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XPAR64 :: doit(void)
{
	bool par[8];

	state->ctx.activations++;

	par[0] = par[1] = par[2] = par[3] =
	    par[4] = par[5] = par[6] = par[7] = IS_L(pin1);

	#define PIN(bit, pin_in) par[bit] ^= IS_H(pin_in);
	XPAR64_PINS();
	#undef PIN

	#define PIN(bit, pin_in) << pin_in
	TRACE(
		<< " odd " << pin1
		<< " "
		XPAR64_PINS()
		<< " | "
		<< par[0] << par[1] << par[2] << par[3]
		<< par[4] << par[5] << par[6] << par[7]
	);
	#undef PIN

	pin66 = AS(par[0]);
	pin67 = AS(par[1]);
	pin68 = AS(par[2]);
	pin69 = AS(par[3]);
	pin70 = AS(par[4]);
	pin71 = AS(par[5]);
	pin72 = AS(par[6]);
	pin73 = AS(par[7]);
	pin74 = AS(
	    par[0] ^
	    par[1] ^
	    par[2] ^
	    par[3] ^
	    par[4] ^
	    par[5] ^
	    par[6] ^
	    par[7] ^
	    IS_L(pin1)
	);
}
