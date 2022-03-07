#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XPAR67.hh"

struct scm_xpar67_state {
	struct ctx ctx;
};

SCM_XPAR67 :: SCM_XPAR67(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);

	#define PIN(bit, pin_in) << pin_in
	sensitive XPAR67_PINS();
	#undef PIN

	state = (struct scm_xpar67_state *)CTX_Get("XPAR67", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XPAR67 :: doit(void)
{
	bool par[9];

	state->ctx.activations++;

	par[0] = par[6] = par[8] = false;
	par[1] = par[2] = par[3] = par[4] = par[5] = par[7] = true;

	#define PIN(bit, pin_in) par[bit] ^= IS_H(pin_in);
	XPAR67_PINS();
	#undef PIN

	#define PIN(bit, pin_in) << pin_in
	TRACE(
		XPAR67_PINS()
		<< " | "
		<< par[0] << par[1] << par[2] << par[3]
		<< par[4] << par[5] << par[6] << par[7] << par[8]
	);
	#undef PIN

	pin70 = AS(par[0]);
	pin71 = AS(par[1]);
	pin72 = AS(par[2]);
	pin73 = AS(par[3]);
	pin74 = AS(par[4]);
	pin75 = AS(par[5]);
	pin76 = AS(par[6]);
	pin77 = AS(par[7]);
	pin78 = AS(par[8]);
	pin79 = AS(!par[0]);
	pin80 = AS(!par[1]);
	pin81 = AS(!par[2]);
	pin82 = AS(!par[3]);
	pin83 = AS(!par[4]);
	pin84 = AS(!par[5]);
	pin85 = AS(!par[6]);
	pin86 = AS(!par[7]);
	pin87 = AS(!par[8]);
}
