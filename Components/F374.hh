// Octal D-Type Flip-Flop with 3-STATE Outputs
// Fairchild DS009524 May 1988 Revised September 2000

#ifndef R1000_F374
#define R1000_F374

#include "F374_pins.hh"

#ifdef ANON_PINS
#define PIN_PAIRS(macro) \
	macro(7, PIN_D0, PIN_Q0) \
	macro(6, PIN_D1, PIN_Q1) \
	macro(5, PIN_D2, PIN_Q2) \
	macro(4, PIN_D3, PIN_Q3) \
	macro(3, PIN_D4, PIN_Q4) \
	macro(2, PIN_D5, PIN_Q5) \
	macro(1, PIN_D6, PIN_Q6) \
	macro(0, PIN_D7, PIN_Q7)
#endif

struct scm_f374_state;

SC_MODULE(SCM_F374)
{
	sc_in <sc_logic>	F374_PIN_CLK;
	sc_in <sc_logic>	F374_PIN_OE;
	sc_in <sc_logic>	F374_PIN_D0;
	sc_in <sc_logic>	F374_PIN_D1;
	sc_in <sc_logic>	F374_PIN_D2;
	sc_in <sc_logic>	F374_PIN_D3;
	sc_in <sc_logic>	F374_PIN_D4;
	sc_in <sc_logic>	F374_PIN_D5;
	sc_in <sc_logic>	F374_PIN_D6;
	sc_in <sc_logic>	F374_PIN_D7;
	sc_out <sc_logic>	F374_PIN_Q0;
	sc_out <sc_logic>	F374_PIN_Q1;
	sc_out <sc_logic>	F374_PIN_Q2;
	sc_out <sc_logic>	F374_PIN_Q3;
	sc_out <sc_logic>	F374_PIN_Q4;
	sc_out <sc_logic>	F374_PIN_Q5;
	sc_out <sc_logic>	F374_PIN_Q6;
	sc_out <sc_logic>	F374_PIN_Q7;

	SC_HAS_PROCESS(SCM_F374);

	SCM_F374(sc_module_name nm, const char *arg);

	private:
	struct scm_f374_state *state;
	void doit(void);
};

#endif /* R1000_F374 */
