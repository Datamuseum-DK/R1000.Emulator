#ifndef R1000_XSR8
#define R1000_XSR8

#include "XSR8_pins.hh"

// 4-bit bidirectional universal shift register
// Philips 1989 Apr 04, IC15 Data Handbook


struct scm_xsr8_state;

SC_MODULE(SCM_XSR8)
{

	sc_in <sc_logic>	XSR8_PIN_CLK;
	sc_in <sc_logic>	XSR8_PIN_CLR;
	sc_in <sc_logic>	XSR8_PIN_RSI;
	sc_in <sc_logic>	XSR8_PIN_LSI;
	sc_in <sc_logic>	XSR8_PIN_S0;
	sc_in <sc_logic>	XSR8_PIN_S1;
	sc_in <sc_logic>	XSR8_PIN_D0;
	sc_in <sc_logic>	XSR8_PIN_D1;
	sc_in <sc_logic>	XSR8_PIN_D2;
	sc_in <sc_logic>	XSR8_PIN_D3;
	sc_in <sc_logic>	XSR8_PIN_D4;
	sc_in <sc_logic>	XSR8_PIN_D5;
	sc_in <sc_logic>	XSR8_PIN_D6;
	sc_in <sc_logic>	XSR8_PIN_D7;

	sc_out <sc_logic>	XSR8_PIN_Q0;
	sc_out <sc_logic>	XSR8_PIN_Q1;
	sc_out <sc_logic>	XSR8_PIN_Q2;
	sc_out <sc_logic>	XSR8_PIN_Q3;
	sc_out <sc_logic>	XSR8_PIN_Q4;
	sc_out <sc_logic>	XSR8_PIN_Q5;
	sc_out <sc_logic>	XSR8_PIN_Q6;
	sc_out <sc_logic>	XSR8_PIN_Q7;

	SC_HAS_PROCESS(SCM_XSR8);

	SCM_XSR8(sc_module_name nm, const char *arg);

	private:
	struct scm_xsr8_state *state;
	void doit(void);
};

#endif /* R1000_XSR8 */
