#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F182.hh"

// Carry Lookahead Generator
// Fairchild DS009492 April 1988 Revised June 2002

static const uint8_t lut182[512] = {
#include "F182_tbl.h"
};

struct scm_f182_state {
	struct ctx ctx;
};

SCM_F182 :: SCM_F182(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin6
		  << pin13 << pin14 << pin15;

	state = (struct scm_f182_state *)
	    CTX_Get("f182", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F182 :: doit(void)
{
	unsigned adr = 0;

	state->ctx.activations++;
	if (IS_H(pin13)) adr |= 1 << 8;
	if (IS_H(pin3)) adr |= 1 << 7;
	if (IS_H(pin4)) adr |= 1 << 6;
	if (IS_H(pin1)) adr |= 1 << 5;
	if (IS_H(pin2)) adr |= 1 << 4;
	if (IS_H(pin14)) adr |= 1 << 3;
	if (IS_H(pin15)) adr |= 1 << 2;
	if (IS_H(pin5)) adr |= 1 << 1;
	if (IS_H(pin6)) adr |= 1 << 0;
	unsigned val = lut182[adr];
	TRACE(
	    << pin1
	    << pin2
	    << pin3
	    << pin4
	    << pin5
	    << pin6
	    << pin13
	    << pin14
	    << pin15
	    << "|"
	    << std::hex << val
	);
	pin12 = AS(val & 0x10);
	pin11 = AS(val & 0x8);
	pin9 = AS(val & 0x4);
	pin10 = AS(val & 0x2);
	pin7 = AS(val & 0x1);
}
