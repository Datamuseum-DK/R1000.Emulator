#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS
#include "XPAXXXL.hh"

// TBP28SA42 4096 (512 Words by 8 bits) PROM (Open-Collector)
// 512x8 PROM
// With a F374 output latch

struct scm_xpaxxxl_state {
	struct ctx ctx;
	unsigned data;
	int job;
};

SCM_XPAXXXL :: SCM_XPAXXXL(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << PIN_CLK.pos();

	state = (struct scm_xpaxxxl_state *)
	    CTX_Get("xpaxxxl", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
	load_programmable(this->name(), prom, sizeof prom, arg);
}

void
SCM_XPAXXXL :: doit(void)
{

	unsigned adr = 0;

	state->ctx.activations++;

	if (state->job) {
		PIN_Y0 = AS(state->data & 0x80);
		PIN_Y1 = AS(state->data & 0x40);
		PIN_Y2 = AS(state->data & 0x20);
		PIN_Y3 = AS(state->data & 0x10);
		PIN_Y4 = AS(state->data & 0x08);
		PIN_Y5 = AS(state->data & 0x04);
		PIN_Y6 = AS(state->data & 0x02);
		PIN_Y7 = AS(state->data & 0x01);
		state->job = 0;
	}

	if (IS_H(PIN_A0)) adr |= 1 << 8;
	if (IS_H(PIN_A1)) adr |= 1 << 7;
	if (IS_H(PIN_A2)) adr |= 1 << 6;
	if (IS_H(PIN_A3)) adr |= 1 << 5;
	if (IS_H(PIN_A4)) adr |= 1 << 4;
	if (IS_H(PIN_A5)) adr |= 1 << 3;
	if (IS_H(PIN_A6)) adr |= 1 << 2;
	if (IS_H(PIN_A7)) adr |= 1 << 1;
	if (IS_H(PIN_A8)) adr |= 1 << 0;
	unsigned data = prom[adr];

	if (data != state->data) {
		TRACE(
		    << " a "
		    << PIN_A0
		    << PIN_A1
		    << PIN_A2
		    << PIN_A3
		    << PIN_A4
		    << PIN_A5
		    << PIN_A6
		    << PIN_A7
		    << PIN_A8
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
		state->data = data;
		state->job = 1;
		next_trigger(5, SC_NS);
	}
}
