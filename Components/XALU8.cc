#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XALU8.hh"

// 4-Bit Arithmetic Logic Unit
// Fairchild DS009491 April 1988 Revised January 2004
// http://www.righto.com/2017/03/inside-vintage-74181-alu-chip-how-it.html

static const uint8_t lut181[16384] = {
#include "F181_tbl.h"
};

struct scm_xalu8_state {
	struct ctx ctx;
};

SCM_XALU8 :: SCM_XALU8(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin6 << pin7 << pin8
		  << pin9 << pin10 << pin11 << pin12 << pin13 << pin14 << pin15 << pin16
		  << pin25 << pin28 << pin29 << pin30 << pin31 << pin32;

	state = (struct scm_xalu8_state *)
	    CTX_Get("xalu8", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XALU8 :: doit(void)
{
	unsigned idx0 = 0;

	state->ctx.activations++;

	if (IS_H(pin25)) idx0 |= 1 << 13; // Ci
	if (IS_H(pin28)) idx0 |= 1 << 12; // M

	if (IS_H(pin5)) idx0 |= 1 << 11; // A4
	if (IS_H(pin6)) idx0 |= 1 << 10; // A5
	if (IS_H(pin7)) idx0 |= 1 << 9;	// A6
	if (IS_H(pin8)) idx0 |= 1 << 8;	// A7

	if (IS_H(pin13)) idx0 |= 1 << 7; // B4
	if (IS_H(pin14)) idx0 |= 1 << 6; // B5
	if (IS_H(pin15)) idx0 |= 1 << 5; // B6
	if (IS_H(pin16)) idx0 |= 1 << 4; // B7

	if (IS_H(pin29)) idx0 |= 1 << 3; // S3
	if (IS_H(pin30)) idx0 |= 1 << 2; // S2
	if (IS_H(pin31)) idx0 |= 1 << 1; // S1
	if (IS_H(pin32)) idx0 |= 1 << 0; // S0

	unsigned val0 = lut181[idx0];

	unsigned idx1 = idx0 & 0x100f;

	if (val0 & 0x02) idx1 |= (1 << 13); // Cn

	if (IS_H(pin1)) idx1 |= 1 << 11; // A0
	if (IS_H(pin2)) idx1 |= 1 << 10; // A1
	if (IS_H(pin3)) idx1 |= 1 << 9;	// A2
	if (IS_H(pin4)) idx1 |= 1 << 8;	// A3

	if (IS_H(pin9)) idx1 |= 1 << 7; // B0
	if (IS_H(pin10)) idx1 |= 1 << 6; // B1
	if (IS_H(pin11)) idx1 |= 1 << 5; // B2
	if (IS_H(pin12)) idx1 |= 1 << 4; // B3

	unsigned val1 = lut181[idx1];

	pin17 = AS(val1 & 0x80);
	pin18 = AS(val1 & 0x40);
	pin19 = AS(val1 & 0x20);
	pin20 = AS(val1 & 0x10);

	pin21 = AS(val0 & 0x80);
	pin22 = AS(val0 & 0x40);
	pin23 = AS(val0 & 0x20);
	pin24 = AS(val0 & 0x10);

	if ((val0 & 0x08) && (val1 & 0x08))
		pin27 = sc_logic_1;
	else
		pin27 = sc_logic_0;

	pin26 = AS(val1 & 0x02);

	TRACE(
	    << " m " << pin28
	    << " s " << pin29 << pin30 << pin31 << pin32
	    << " ci " << pin25
	    << " a " << pin1 << pin2 << pin3 << pin4 << pin5 << pin6 << pin7 << pin8
	    << " b " << pin9 << pin10 << pin11 << pin12 << pin13 << pin14 << pin15 << pin16
	    << " idx0 " << std::hex << idx0
	    << " idx1 " << std::hex << idx1
	    << " = " << ((val0 & 0x08) && (val1 & 0x08))
	    << " co " << AS(val1 & 0x02)
	);
}
