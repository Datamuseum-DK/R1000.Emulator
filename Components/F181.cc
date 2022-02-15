#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F181.hh"

// 4-Bit Arithmetic Logic Unit
// Fairchild DS009491 April 1988 Revised January 2004
// http://www.righto.com/2017/03/inside-vintage-74181-alu-chip-how-it.html

static const uint8_t lut181[16384] = {
#include "F181_tbl.h"
};

struct scm_f181_state {
	struct ctx ctx;
};

void
SCM_F181 :: loadit(const char *arg)
{
	state = (struct scm_f181_state *)
	    CTX_Get("f181", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_F181 :: doit(void)
{
	unsigned idx = 0;

	state->ctx.activations++;
	if (IS_H(pin7)) idx |= 1 << 13;	// Cn
	if (IS_H(pin8)) idx |= 1 << 12;	// M
	if (IS_H(pin19)) idx |= 1 << 11; // A3
	if (IS_H(pin21)) idx |= 1 << 10; // A2
	if (IS_H(pin23)) idx |= 1 << 9;	// A1
	if (IS_H(pin2)) idx |= 1 <<  8;	// A0
	if (IS_H(pin18)) idx |= 1 << 7; // B3
	if (IS_H(pin20)) idx |= 1 << 6;	// B2
	if (IS_H(pin22)) idx |= 1 << 5;	// B1
	if (IS_H(pin1)) idx |= 1 << 4;	// B0
	if (IS_H(pin3)) idx |= 1 << 3;	// S3
	if (IS_H(pin4)) idx |= 1 << 2;	// S2
	if (IS_H(pin5)) idx |= 1 << 1;	// S1
	if (IS_H(pin6)) idx |= 1 << 0;	// S0
	unsigned val = lut181[idx];
	pin13 = AS(val & 0x80);
	pin11 = AS(val & 0x40);
	pin10 = AS(val & 0x20);
	pin9 = AS(val & 0x10);
	if (val & 0x08)
		pin14 = sc_logic_Z;
	else
		pin14 = sc_logic_0;
	pin15 = AS(val & 0x04);
	pin16 = AS(val & 0x02);
	pin17 = AS(val & 0x01);

	TRACE(
	    << " s " << pin3 << pin4 << pin5 << pin6
	    << " m " << pin8
	    << " cn " << pin7
	    << " a " << pin19 << pin21 << pin23 << pin2
	    << " b " << pin18 << pin20 << pin22 << pin1
	    << " idx " << std::hex << idx
	    << " f " << AS(val & 0x80) << AS(val & 0x40) << AS(val & 0x20) << AS(val & 0x10)
	    << " = " << AS(val & 0x08)
	    << " p " << AS(val & 0x04)
	    << " cn " << AS(val & 0x02)
	    << " g " << AS(val & 0x01)
	);
}
