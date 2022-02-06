#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "Components/XIOCRAM.hh"

struct scm_xiocram_state {
	struct ctx ctx;
};

SCM_XIOCRAM :: SCM_XIOCRAM(sc_module_name nm, const char *arg) : sc_module(nm)
{
	cout << "MISSING SCM_XIOCRAM IMPLEMENTATION\n";
	SC_METHOD(doit);

	state = (struct scm_xiocram_state *)CTX_Get("XIOCRAM", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);
}

void
SCM_XIOCRAM :: doit(void)
{
	state->ctx.activations++;
	TRACE(
	    <<pin1
	    <<pin2
	    <<pin3
	    <<pin4
	    <<pin5
	    <<pin6
	    <<pin7
	    <<pin8
	    <<pin9
	    <<pin10
	    <<pin11
	    <<pin12
	    <<pin13
	    <<pin14
	    <<pin15
	    <<pin16
	    <<pin17
	    <<pin18
	    <<pin19
	    <<pin20
	    <<pin21
	    <<pin22
	    <<pin23
	    <<pin24
	    <<pin25
	    <<pin26
	    <<pin27
	    <<pin28
	    <<pin29
	    <<pin30
	    <<pin31
	    <<pin32
	    <<pin33
	    <<pin34
	    <<pin35
	    <<pin36
	    <<pin37
	    <<pin38
	    <<pin39
	    <<pin40
	    <<pin41
	    <<pin42
	    <<pin43
	);
}
