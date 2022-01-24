#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "r1000sc_priv.h"
#include "PAxxx.hh"

// TBP28SA42 4096 (512 Words by 8 bits) PROM (Open-Collector)
// 512x8 PROM

struct scm_paxxx_state {
	struct ctx ctx;
};

void
SCM_PAXXX :: loadit(const char *arg)
{
	state = (struct scm_paxxx_state *)
	    CTX_Get("paxxx", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	load_programmable(this->name(), prom, sizeof prom, arg);
}

void
SCM_PAXXX :: doit(void)
{
	unsigned adr = 0;

	state->ctx.activations++;
	if (IS_H(pin1)) adr |= 1 << 0;
	if (IS_H(pin2)) adr |= 1 << 1;
	if (IS_H(pin3)) adr |= 1 << 2;
	if (IS_H(pin4)) adr |= 1 << 3;
	if (IS_H(pin5)) adr |= 1 << 4;
	if (IS_H(pin16)) adr |= 1 << 5;
	if (IS_H(pin17)) adr |= 1 << 6;
	if (IS_H(pin18)) adr |= 1 << 7;
	if (IS_H(pin19)) adr |= 1 << 8;
	unsigned data = prom[adr];
	TRACE(
	    << " oe_ " << pin15
	    << " a "
	    <<pin19 <<pin18 <<pin17 <<pin16 <<pin5 <<pin4 <<pin3 <<pin2 <<pin1
	    << " d "
	    << AS(data & 0x80)
	    << AS(data & 0x40)
	    << AS(data & 0x20)
	    << AS(data & 0x10)
	    << AS(data & 0x08)
	    << AS(data & 0x04)
	    << AS(data & 0x02)
	    << AS(data & 0x01)
	);
	if (IS_L(pin15)) {
		pin14 = AS(data & 0x80);
		pin13 = AS(data & 0x40);
		pin12 = AS(data & 0x20);
		pin11 = AS(data & 0x10);
		pin9 = AS(data & 0x08);
		pin8 = AS(data & 0x04);
		pin7 = AS(data & 0x02);
		pin6 = AS(data & 0x01);
	} else {
		pin14 = sc_logic_Z;
		pin13 = sc_logic_Z;
		pin12 = sc_logic_Z;
		pin11 = sc_logic_Z;
		pin9 = sc_logic_Z;
		pin8 = sc_logic_Z;
		pin7 = sc_logic_Z;
		pin6 = sc_logic_Z;
	}
}
