// N=32 bit two input mux with individual selects

#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"

#define ANON_PINS

#include "XMUX232.hh"

SC_MODULE(SCM_MUX2)
{
	sc_in <sc_logic>	a;
	sc_in <sc_logic>	b;
	sc_in <sc_logic>	s;
	sc_out <sc_logic>	q;

	SC_HAS_PROCESS(SCM_MUX2);

	SCM_MUX2(sc_module_name nm, uint64_t *up_act_cnt) : sc_module(nm)
	{
		act_cnt = up_act_cnt;
		SC_METHOD(doit);
		sensitive << a << b << s;
	}

	void
	doit(void)
	{
		(*act_cnt)++;
		if (IS_H(s))
			q = AS(IS_H(b));
		else
			q = AS(IS_H(a));
	}

	uint64_t *act_cnt;
};

struct scm_xmux232_state {
	struct ctx ctx;
	SCM_MUX2 *bit[32];
};

SCM_XMUX232 :: SCM_XMUX232(sc_module_name nm, const char *arg) : sc_module(nm)
{
	char buf[80];

	state = (struct scm_xmux232_state *)CTX_Get("XMUX232", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);

	#define PINM(n, pin_a, pin_b, pin_s, pin_q) \
	    sprintf(buf, "bit%d", n); \
	    state->bit[n] = new SCM_MUX2(buf, &state->ctx.activations); \
	    state->bit[n]->a(pin_a); \
	    state->bit[n]->b(pin_b); \
	    state->bit[n]->s(pin_s); \
	    state->bit[n]->q(pin_q);
	PIN_PAIRS(PINM);
	#undef PINM
}
