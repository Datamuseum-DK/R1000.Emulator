#include <systemc.h>
#include "Chassis/r1000sc.h"
#include "Infra/context.h"
#include "F245.hh"

// Octal Bidirectional Transceiver with 3-STATE Outputs
// Fairchild Rev 1.4.0 January 2008

SC_MODULE(SCM_F245_5)
{
	sc_in <sc_logic>	pin_oe;
	sc_in <sc_logic>	pin_dir;
	sc_in <sc_logic>	pin_d0;
	sc_in <sc_logic>	pin_d1;
	sc_in <sc_logic>	pin_d2;
	sc_in <sc_logic>	pin_d3;
	sc_in <sc_logic>	pin_d4;
	sc_in <sc_logic>	pin_d5;
	sc_in <sc_logic>	pin_d6;
	sc_in <sc_logic>	pin_d7;
	sc_out <sc_logic>	pin_y0;
	sc_out <sc_logic>	pin_y1;
	sc_out <sc_logic>	pin_y2;
	sc_out <sc_logic>	pin_y3;
	sc_out <sc_logic>	pin_y4;
	sc_out <sc_logic>	pin_y5;
	sc_out <sc_logic>	pin_y6;
	sc_out <sc_logic>	pin_y7;

	SC_HAS_PROCESS(SCM_F245_5);

	SCM_F245_5(sc_module_name nm, uint64_t *up_act_cnt, bool dir) :
	    sc_module(nm)
	{
		act_cnt = up_act_cnt;
		mydir = dir;
		SC_METHOD(doit);
		sensitive << pin_oe << pin_dir << pin_d0 << pin_d1 <<
		    pin_d2 << pin_d3 << pin_d4 << pin_d5 << pin_d6 << pin_d7;
	}

	uint64_t *act_cnt;
	bool mydir;

	void
	doit(void)
	{
		(*act_cnt)++;
		if (IS_H(pin_oe)) {
			pin_y0 = sc_logic_Z;
			pin_y1 = sc_logic_Z;
			pin_y2 = sc_logic_Z;
			pin_y3 = sc_logic_Z;
			pin_y4 = sc_logic_Z;
			pin_y5 = sc_logic_Z;
			pin_y6 = sc_logic_Z;
			pin_y7 = sc_logic_Z;
			next_trigger(pin_oe.negedge_event());
		} else if (mydir ^ IS_H(pin_dir)) {
			pin_y0 = sc_logic_Z;
			pin_y1 = sc_logic_Z;
			pin_y2 = sc_logic_Z;
			pin_y3 = sc_logic_Z;
			pin_y4 = sc_logic_Z;
			pin_y5 = sc_logic_Z;
			pin_y6 = sc_logic_Z;
			pin_y7 = sc_logic_Z;
			next_trigger(pin_dir.default_event());
		} else {
			pin_y0 = pin_d0;
			pin_y1 = pin_d1;
			pin_y2 = pin_d2;
			pin_y3 = pin_d3;
			pin_y4 = pin_d4;
			pin_y5 = pin_d5;
			pin_y6 = pin_d6;
			pin_y7 = pin_d7;
		}
	}
};

struct scm_f245_state {
	struct ctx ctx;
	SCM_F245_5 *ab;
	SCM_F245_5 *ba;
};

SCM_F245 :: SCM_F245(sc_module_name nm, const char *arg) : sc_module(nm)
{

	state = (struct scm_f245_state *)
	    CTX_Get("f245", this->name(), sizeof *state);
	should_i_trace(this->name(), &state->ctx.do_trace);

	state->ab = new SCM_F245_5("ab", &state->ctx.activations, true);
	state->ba = new SCM_F245_5("ba", &state->ctx.activations, false);
	state->ab->pin_oe(pin19);	state->ba->pin_oe(pin19);
	state->ab->pin_dir(pin1);	state->ba->pin_dir(pin1);
	state->ab->pin_d0(pin2);	state->ba->pin_y0(pin2);
	state->ab->pin_d1(pin3);	state->ba->pin_y1(pin3);
	state->ab->pin_d2(pin4);	state->ba->pin_y2(pin4);
	state->ab->pin_d3(pin5);	state->ba->pin_y3(pin5);
	state->ab->pin_d4(pin6);	state->ba->pin_y4(pin6);
	state->ab->pin_d5(pin7);	state->ba->pin_y5(pin7);
	state->ab->pin_d6(pin8);	state->ba->pin_y6(pin8);
	state->ab->pin_d7(pin9);	state->ba->pin_y7(pin9);
	state->ab->pin_y0(pin18);	state->ba->pin_d0(pin18);
	state->ab->pin_y1(pin17);	state->ba->pin_d1(pin17);
	state->ab->pin_y2(pin16);	state->ba->pin_d2(pin16);
	state->ab->pin_y3(pin15);	state->ba->pin_d3(pin15);
	state->ab->pin_y4(pin14);	state->ba->pin_d4(pin14);
	state->ab->pin_y5(pin13);	state->ba->pin_d5(pin13);
	state->ab->pin_y6(pin12);	state->ba->pin_d6(pin12);
	state->ab->pin_y7(pin11);	state->ba->pin_d7(pin11);
}
