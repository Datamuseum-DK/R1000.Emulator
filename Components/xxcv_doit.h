
void
XXCV_CLS :: doit(void)
{
	state->ctx.activations++;
	uint64_t a = 0;
	uint64_t b = 0;
	char what[12];

	what[0] = '\0';
	if (IS_H(XXCV_OEA)) {
		#define PIN(nbr, pin_a, pin_b) pin_a = sc_logic_Z;
		XXCV_PINS()
		#undef PIN

		#define PIN(nbr, pin_a, pin_b) if (IS_H(pin_a)) a |= ((uint64_t)1 << (XXCV_LPIN - nbr));
		XXCV_PINS()
		#undef PIN

		strcat(what, "Ai");
	}
	if (IS_L(XXCV_OEB)) {
		#define PIN(nbr, pin_a, pin_b) pin_b = sc_logic_Z;
		XXCV_PINS()
		#undef PIN

		#define PIN(nbr, pin_a, pin_b) if (IS_H(pin_b)) b |= ((uint64_t)1 << (XXCV_LPIN - nbr));
		XXCV_PINS()
		#undef PIN

		strcat(what, "Bi");
	}
	if (IS_H(XXCV_OEB)) {
		if (IS_L(XXCV_SAB)) {
			b = a;
			strcat(what, "Ba");
		} else {
			b = state->areg;
			strcat(what, "Br");
		}
	}
	if (IS_L(XXCV_OEA)) {
		if (IS_L(XXCV_SBA)) {
			a = b;
			strcat(what, "Ab");
		} else {
			a = state->breg;
			strcat(what, "Ar");
		}
	}

	if (XXCV_CAB.posedge()) {
		state->areg = a;
		strcat(what, "A<");
	}
	if (XXCV_CBA.posedge()) {
		state->breg = b;
		strcat(what, "B<");
	}

	if (IS_L(XXCV_OEA)) {
		// A is output
		#define PIN(nbr, pin_a, pin_b) pin_a = AS(a & ((uint64_t)1 << (XXCV_LPIN - nbr)));
		XXCV_PINS()
		#undef PIN

		strcat(what, "A>");
	}
	if (IS_H(XXCV_OEB)) {
		// B is output
		#define PIN(nbr, pin_a, pin_b) pin_b = AS(b & ((uint64_t)1 << (XXCV_LPIN - nbr)));
		XXCV_PINS()
		#undef PIN

		strcat(what, "B>");
	}
	TRACE(
	    << " oea " << XXCV_OEA
	    << " cba " << XXCV_CBA
	    << " sba " << XXCV_SBA
	    << " oeb " << XXCV_OEB
	    << " cab " << XXCV_CAB
	    << " sab " << XXCV_SAB
	    << " | a "
	    << std::hex << a
	    << " ra "
	    << std::hex << state->areg
	    << " b "
	    << std::hex << b
	    << " rb "
	    << std::hex << state->breg
	);
}
