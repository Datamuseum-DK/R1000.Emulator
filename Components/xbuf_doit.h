
#ifndef XBUF_DOIT
#define XBUF_DOIT doit
#endif
#ifndef XBUF_OE
#define XBUF_OE pin1
#endif
#ifndef XBUF_JOB
#define XBUF_JOB job
#endif
#ifndef XBUF_TRACE
#define XBUF_TRACE
#endif

void
XBUF_CLASS :: XBUF_DOIT(void)
{
	state->ctx.activations++;

	#define PIN(bit,pin_in,pin_out) << pin_in
	if (state->XBUF_JOB || (state->ctx.do_trace & 2)) {
		TRACE( XBUF_TRACE << "job " << state->XBUF_JOB << " oe " << XBUF_OE
		    << " d " XBUF_PINS());
	}
	#undef PIN
	if (state->XBUF_JOB > 0) {
		#define PIN(bit,pin_in,pin_out) pin_out = AS(state->out[bit]);
		XBUF_PINS()
		#undef PIN
		state->XBUF_JOB = 0;
	}
	if (state->XBUF_JOB < 0) {
		#define PIN(bit,pin_in,pin_out) pin_out = sc_logic_Z;
		XBUF_PINS()
		#undef PIN
	}
	if (IS_H(XBUF_OE)) {
		#define PIN(bit,pin_in,pin_out) \
		pin_out = sc_logic_Z;
		XBUF_PINS()
		#undef PIN
		state->XBUF_JOB = -1;
		next_trigger(XBUF_OE.negedge_event());
	} else {
		bool tmp;

		if (state->XBUF_JOB < 0)
			state->XBUF_JOB = 1;
		#define PIN(bit,pin_in,pin_out) \
		tmp = XBUF_SENSE(pin_in); \
		if (tmp != state->out[bit]) { \
			state->XBUF_JOB = 1; \
			state->out[bit] = tmp; \
		}
		XBUF_PINS()
		#undef PIN
		if (state->XBUF_JOB)
			next_trigger(5, SC_NS);
	}
}
