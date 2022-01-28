#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F283.hh"

// 4-Bit Binary Full Adder with Fast Carry
// Fairchild DS009513 April 1988 Revised January 2004

struct scm_f283_state {
	struct ctx ctx;
};

void
SCM_F283 :: loadit(const char *arg)
{
	state = (struct scm_f283_state *)
	    CTX_Get("f283", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F283 :: doit(void)
{
	unsigned sum = 0;

	state->ctx.activations++;
	if (IS_H(pin7)) sum += 1;
	if (IS_H(pin5)) sum += 1;
	if (IS_H(pin6)) sum += 1;
	if (IS_H(pin2)) sum += 2;
	if (IS_H(pin3)) sum += 2;
	if (IS_H(pin14)) sum += 4;
	if (IS_H(pin15)) sum += 4;
	if (IS_H(pin12)) sum += 8;
	if (IS_H(pin11)) sum += 8;
	TRACE(
	    << " a " << pin12 << pin14 << pin3 << pin5
	    << " b " << pin11 << pin15 << pin2 << pin6
	    << " ci " << pin7
	    << " sum "
	    << std::hex << sum
	);
	pin4 = AS(sum & 1);
	pin1 = AS(sum & 2);
	pin13 = AS(sum & 4);
	pin10 = AS(sum & 8);
	pin9 = AS(sum & 16);
}
