
void
XBUF_CLASS :: doit(void)
{
	state->ctx.activations++;

	#define PIN(bit,pin_in,pin_out) << pin_in
	if (state->job || (state->ctx.do_trace & 2)) {
		TRACE(<< "job " << state->job << " e " << pin1
		    << " d " XBUF_PINS());
	}
	#undef PIN
	if (state->job > 0) {
		#define PIN(bit,pin_in,pin_out) pin_out = AS(state->out[bit]);
		XBUF_PINS()
		#undef PIN
		state->job = 0;
	}

	if (IS_H(pin1)) {
		#define PIN(bit,pin_in,pin_out) \
		pin_out = sc_logic_Z;
		XBUF_PINS()
		#undef PIN
		state->job = -1;
		next_trigger(pin1.negedge_event());
	} else {
		bool tmp;

		if (state->job < 0)
			state->job = 1;
		#define PIN(bit,pin_in,pin_out) \
		tmp = XBUF_SENSE(pin_in); \
		if (tmp != state->out[bit]) { \
			state->job = 1; \
			state->out[bit] = tmp; \
		}
		XBUF_PINS()
		#undef PIN
		if (state->job)
			next_trigger(5, SC_NS);
	}
}
