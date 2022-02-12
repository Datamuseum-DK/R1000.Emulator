#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "PAxxxL.hh"

// TBP28SA42 4096 (512 Words by 8 bits) PROM (Open-Collector)
// 512x8 PROM
// With a F374 output latch

struct scm_paxxxl_state {
	struct ctx ctx;
	unsigned olddata;
};

SCM_PAXXXL :: SCM_PAXXXL(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin15.pos();

	state = (struct scm_paxxxl_state *)
	    CTX_Get("paxxxl", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	load_programmable(this->name(), prom, sizeof prom, arg);
}

void
SCM_PAXXXL :: doit(void)
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

	if (data != state->olddata) {
		TRACE(
		    << " a "
		    <<pin19 <<pin18 <<pin17 <<pin16 <<pin5
		    <<pin4 <<pin3 <<pin2 <<pin1
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
		state->olddata = data;

		pin14 = AS(data & 0x80);
		pin13 = AS(data & 0x40);
		pin12 = AS(data & 0x20);
		pin11 = AS(data & 0x10);
		pin9 = AS(data & 0x08);
		pin8 = AS(data & 0x04);
		pin7 = AS(data & 0x02);
		pin6 = AS(data & 0x01);
	}
}
