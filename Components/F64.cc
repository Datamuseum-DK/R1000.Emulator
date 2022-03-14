#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F64.hh"

// 4-2-3-2-Input AND-OR-Invert Gate
// Fairchild DS009467 April 1988 Revised March 1999

SC_MODULE(SCM_F64_2)
{
	sc_in <sc_logic>	term1;
	sc_in <sc_logic>	term2;
	sc_in <sc_logic>	term3;
	sc_in <sc_logic>	term4;
	sc_out <bool>		term5;

	SC_HAS_PROCESS(SCM_F64_2);

	SCM_F64_2(sc_module_name nm, uint64_t *up_act_cnt) : sc_module(nm)
	{
		act_cnt = up_act_cnt;
		SC_METHOD(doit);
		sensitive << term1 << term2 << term3 << term4;
	}

	void
	doit(void)
	{
		(*act_cnt)++;
		term5 = IS_H(term1) && IS_H(term2) && IS_H(term3) && IS_H(term4);
		if (term1 != sc_logic_1)
			next_trigger(term1.posedge_event());
		else if (term2 != sc_logic_1)
			next_trigger(term2.posedge_event());
		else if (term3 != sc_logic_1)
			next_trigger(term3.posedge_event());
		else if (term4 != sc_logic_1)
			next_trigger(term4.posedge_event());
	}

	uint64_t *act_cnt;
};

struct scm_f64_state {
	struct ctx ctx;
	SCM_F64_2 *input_a;
	SCM_F64_2 *input_b;
	SCM_F64_2 *input_c;
	SCM_F64_2 *input_d;
	sc_signal<bool> *sig_a;
	sc_signal<bool> *sig_b;
	sc_signal<bool> *sig_c;
	sc_signal<bool> *sig_d;
};

SCM_F64 :: SCM_F64(sc_module_name nm, const char *arg) : sc_module(nm)
{
	state = (struct scm_f64_state *)
	    CTX_Get("f64", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);

	state->sig_a = new sc_signal<bool>("sig_a");
	state->sig_b = new sc_signal<bool>("sig_b");
	state->sig_c = new sc_signal<bool>("sig_c");
	state->sig_d = new sc_signal<bool>("sig_d");

	state->input_a = new SCM_F64_2("siga", &state->ctx.activations);
	state->input_a->term1(pin2);
	state->input_a->term2(pin3);
	state->input_a->term3(pin3);
	state->input_a->term4(pin3);
	state->input_a->term5(*(state->sig_a));

	state->input_b = new SCM_F64_2("sigb", &state->ctx.activations);
	state->input_b->term1(pin9);
	state->input_b->term2(pin10);
	state->input_b->term3(pin10);
	state->input_b->term4(pin10);
	state->input_b->term5(*(state->sig_b));

	state->input_c = new SCM_F64_2("sigc", &state->ctx.activations);
	state->input_c->term1(pin4);
	state->input_c->term2(pin5);
	state->input_c->term3(pin6);
	state->input_c->term4(pin6);
	state->input_c->term5(*(state->sig_c));

	state->input_d = new SCM_F64_2("sigd", &state->ctx.activations);
	state->input_d->term1(pin1);
	state->input_d->term2(pin13);
	state->input_d->term3(pin12);
	state->input_d->term4(pin11);
	state->input_d->term5(*(state->sig_d));

	SC_METHOD(doit);
	sensitive << *(state->sig_a) << *(state->sig_b) << *(state->sig_c) << *(state->sig_d);
}

void
SCM_F64 :: doit(void)
{
	bool a = *state->sig_a;
	bool b = *state->sig_b;
	bool c = *state->sig_c;
	bool d = *state->sig_d;
	bool s = !(a || b || c || d);
	TRACE(
	    << a << "="
	    << pin2
	    << pin3
	    << ','
	    << b << "="
	    << pin9
	    << pin10
	    << ','
	    << c << "="
	    << pin4
	    << pin5
	    << pin6
	    << ','
	    << d << "="
	    << pin1
	    << pin11
	    << pin12
	    << pin13
	    << "|"
	    << s
	);
	pin8 = AS(s);
}
