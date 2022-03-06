#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "XXCV9.hh"

struct scm_xxcv9_state {
	struct ctx ctx;
	uint64_t areg;
	uint64_t breg;
};

SCM_XXCV9 :: SCM_XXCV9(sc_module_name nm, const char *arg) : sc_module(nm)
{
	SC_METHOD(doit);
	sensitive << XXCV9_OEA << XXCV9_CBA.pos() << XXCV9_SBA
	    << XXCV9_OEB << XXCV9_CAB.pos() << XXCV9_SAB
#define PIN(nbr, pin_a, pin_b) << pin_a << pin_b
	XXCV9_PINS()
#undef PIN
	;

	state = (struct scm_xxcv9_state *)CTX_Get("XXCV9", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

#define XXCV_CLS SCM_XXCV9
#define XXCV_OEA XXCV9_OEA
#define XXCV_CBA XXCV9_CBA
#define XXCV_SBA XXCV9_SBA
#define XXCV_OEB XXCV9_OEB
#define XXCV_CAB XXCV9_CAB
#define XXCV_SAB XXCV9_SAB
#define XXCV_LPIN XXCV9_LPIN
#define XXCV_PINS XXCV9_PINS

#include "xxcv_doit.h"
