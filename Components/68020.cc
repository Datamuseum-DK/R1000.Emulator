#include <systemc.h>
#include "r1000sc.h"
#include "context.h"
#include "68020.hh"

struct scm_68020_state {
	struct ctx ctx;
};

void
SCM_68020 :: loadit(const char *arg)
{
	state = (struct scm_68020_state *)CTX_Get("68020", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_68020 :: doit(void)
{
	state->ctx.activations++;
	pinF1 = sc_logic_0;	// SIZ0
	pinG2 = sc_logic_0;	// SIZ1
	TRACE(
	    <<pinA1
	    <<pinB3
	    <<pinC1
	    <<pinC2
	    <<pinH1
	    <<pinH2
	    <<pinH3
	    <<pinH12
	    <<pinJ1
	    <<pinJ2
	    <<pinJ12
	    <<pinJ13
	    <<pinK2
	    <<pinK12
	    <<pinK13
	    <<pinL3
	    <<pinL4
	    <<pinL5
	    <<pinL6
	    <<pinL8
	    <<pinL9
	    <<pinL10
	    <<pinL12
	    <<pinL13
	    <<pinM2
	    <<pinM3
	    <<pinM4
	    <<pinM5
	    <<pinM6
	    <<pinM7
	    <<pinM9
	    <<pinM10
	    <<pinM11
	    <<pinM12
	    <<pinM13
	    <<pinN1
	    <<pinN2
	    <<pinN3
	    <<pinN4
	    <<pinN5
	    <<pinN6
	    <<pinN9
	    <<pinN10
	    <<pinN11
	    <<pinN12
	);
}
