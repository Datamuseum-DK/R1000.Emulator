
	state->ctx.activations++;

#define PINM(bit, pin_in, pin_out) << pin_in
	TRACE(
	    << " job " << state->job
	    << " clk " << PIN_CLK.posedge()
	    << " inv " << PIN_INV
	    << " oe " << PIN_OE
	    << " d " PIN_PAIRS(PINM)
	    << " | " << std::hex << state->data
	);
	#undef PINM

	if (state->job > 0) {
		uint64_t tmp = state->data;
		if (IS_L(PIN_INV))
			tmp = ~tmp;
		#define PINM(bit, pin_in, pin_out) \
		pin_out = AS(tmp & ((uint64_t)1 << bit));
		PIN_PAIRS(PINM)
		#undef PINM
		state->job = 0;
	} else if (state->job == -1) {
		#define PINM(bit, pin_in, pin_out) \
		pin_out = sc_logic_Z;
		PIN_PAIRS(PINM)
		#undef PINM
		state->job = -2;
	}

	if (PIN_CLK.posedge()) {
		uint64_t tmp = 0;
		#define PINM(bit,pin_in,pin_out) \
		if (IS_H(pin_in)) tmp |= ((uint64_t)1 << bit);
		PIN_PAIRS(PINM)
		#undef PINM
		if (tmp != state->data) {
			state->data = tmp;
			state->job = -1;
		}
	}

	if (IS_L(PIN_OE)) {
		if (state->job < 0) {
			state->job = 1;
			next_trigger(5, SC_NS);
		}
	} else if (state->job != -2) {
		state->job = -1;
		next_trigger(5, SC_NS);
	}
