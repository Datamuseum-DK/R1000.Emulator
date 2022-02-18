#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XSTKRAM.hh"

struct scm_xstkram_state {
	struct ctx ctx;
	uint16_t ram[16];
};

SCM_XSTKRAM :: SCM_XSTKRAM(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << pin34;

	state = (struct scm_xstkram_state *)CTX_Get("XSTKRAM", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XSTKRAM :: doit(void)
{
	unsigned adr, data;

	state->ctx.activations++;

	#define PIN(bit, pin_in, pin_out) << pin_in
	TRACE(
	    << "oe " << pin34
	    << " we " << pin33
	    << " a " << pin35 << pin36 << pin37 << pin38
	    << " d " XSTKRAM_PINS()
	);
	#undef PIN
	adr = 0;
	if (IS_H(pin35)) adr |= (1<<3);
	if (IS_H(pin36)) adr |= (1<<2);
	if (IS_H(pin37)) adr |= (1<<1);
	if (IS_H(pin38)) adr |= (1<<0);

	if (IS_H(pin34)) {
		TRACE( << " Z");
		#define PIN(bit, pin_in, pin_out) pin_out = sc_logic_Z;
		XSTKRAM_PINS()
		#undef PIN
	} else if (IS_H(pin33)) {
		data = state->ram[adr];
		TRACE( << " r a " << std::hex << adr << " d " << std::hex << data);
		#define PIN(bit, pin_in, pin_out) pin_out = AS(data & (1 << (15-bit)));
		XSTKRAM_PINS()
		#undef PIN
	} else {

		data = 0;
		#define PIN(bit, pin_in, pin_out) if(IS_H(pin_in)) data |= (1 << (15-bit)); pin_out = AS(IS_H(pin_in));
		XSTKRAM_PINS()
		#undef PIN

		state->ram[adr] = data;
		TRACE( << " w a " << std::hex << adr << " d " << std::hex << data);
	}
}
