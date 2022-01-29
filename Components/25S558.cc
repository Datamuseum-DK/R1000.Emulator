#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "25S558.hh"

struct scm_25s558_state {
	struct ctx ctx;
};

SCM_25S558 :: SCM_25S558(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2 << pin3 << pin4 << pin6 << pin7
	    << pin8 << pin12 << pin13 << pin14 << pin15
	    << pin16 << pin17 << pin18 << pin19;

	state = (struct scm_25s558_state *)
	    CTX_Get("25s558", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_25S558 :: doit(void)
{
	unsigned x = 0, y = 0, p = 0;

	state->ctx.activations++;
	if (IS_H(pin1)) x |= (1 << 0);
	if (IS_H(pin2)) x |= (1 << 1);
	if (IS_H(pin3)) x |= (1 << 2);
	if (IS_H(pin4)) x |= (1 << 3);
	if (IS_H(pin5)) x |= (1 << 4);
	if (IS_H(pin6)) x |= (1 << 5);
	if (IS_H(pin7)) x |= (1 << 6);
	if (IS_H(pin8)) x |= (1 << 7);

	if (IS_H(pin12)) y |= (1 << 0);
	if (IS_H(pin13)) y |= (1 << 1);
	if (IS_H(pin14)) y |= (1 << 2);
	if (IS_H(pin15)) y |= (1 << 3);
	if (IS_H(pin16)) y |= (1 << 4);
	if (IS_H(pin17)) y |= (1 << 5);
	if (IS_H(pin18)) y |= (1 << 6);
	if (IS_H(pin19)) y |= (1 << 7);

	p = x * y;

	TRACE(
	    << " x "
	    <<pin8 <<pin7 <<pin6 <<pin5 <<pin4 <<pin3 <<pin2 <<pin1
	    << " y "
	    <<pin19 <<pin18 <<pin17 <<pin16 <<pin15 <<pin14 <<pin13 <<pin12
	    << " x "
	    << std::hex << x
	    << " y "
	    << std::hex << y
	    << " p "
	    << std::hex << p
	);
	pin23 = AS(p & (1 << 15));
	pin24 = AS(p & (1 << 14));
	pin25 = AS(p & (1 << 13));
	pin26 = AS(p & (1 << 12));
	pin27 = AS(p & (1 << 11));
	pin28 = AS(p & (1 << 10));
	pin29 = AS(p & (1 << 9));
	pin31 = AS(p & (1 << 8));
	pin32 = AS(p & (1 << 7));
	pin33 = AS(p & (1 << 6));
	pin34 = AS(p & (1 << 5));
	pin35 = AS(p & (1 << 4));
	pin36 = AS(p & (1 << 3));
	pin37 = AS(p & (1 << 2));
	pin38 = AS(p & (1 << 1));
	pin39 = AS(p & (1 << 0));
}
