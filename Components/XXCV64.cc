#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XXCV64.hh"

struct scm_xxcv64_state {
	struct ctx ctx;
	uint64_t areg;
	uint64_t breg;
};

SCM_XXCV64 :: SCM_XXCV64(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << XXCV64_OEA << XXCV64_CBA.pos() << XXCV64_SBA
	    << XXCV64_OEB << XXCV64_CAB.pos() << XXCV64_SAB
#define PIN(nbr, pin_a, pin_b) << pin_a << pin_b
	XXCV64_PINS()
#undef PIN
	;

	state = (struct scm_xxcv64_state *)CTX_Get("XXCV64", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}
#define XXCV_CLS SCM_XXCV64
#define XXCV_OEA XXCV64_OEA
#define XXCV_CBA XXCV64_CBA
#define XXCV_SBA XXCV64_SBA
#define XXCV_OEB XXCV64_OEB
#define XXCV_CAB XXCV64_CAB
#define XXCV_SAB XXCV64_SAB
#define XXCV_LPIN XXCV64_LPIN
#define XXCV_PINS XXCV64_PINS

#include "xxcv_doit.h"

#if 0

void
SCM_XXCV64 :: doit(void)
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

#endif
