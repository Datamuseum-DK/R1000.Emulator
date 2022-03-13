#ifndef R1000_XSR12
#define R1000_XSR12

#include "XSR12_pins.hh"

// 4-bit bidirectional universal shift register
// Philips 1989 Apr 04, IC15 Data Handbook


struct scm_xsr12_state;

SC_MODULE(SCM_XSR12)
{
	sc_in <sc_logic>	XSR12_PIN_CLK;
	sc_in <sc_logic>	XSR12_PIN_CLR;
	sc_in <sc_logic>	XSR12_PIN_RSI;
	sc_in <sc_logic>	XSR12_PIN_LSI;
	sc_in <sc_logic>	XSR12_PIN_S0;
	sc_in <sc_logic>	XSR12_PIN_S1;
	sc_in <sc_logic>	XSR12_PIN_D0;
	sc_in <sc_logic>	XSR12_PIN_D1;
	sc_in <sc_logic>	XSR12_PIN_D2;
	sc_in <sc_logic>	XSR12_PIN_D3;
	sc_in <sc_logic>	XSR12_PIN_D4;
	sc_in <sc_logic>	XSR12_PIN_D5;
	sc_in <sc_logic>	XSR12_PIN_D6;
	sc_in <sc_logic>	XSR12_PIN_D7;
	sc_in <sc_logic>	XSR12_PIN_D8;
	sc_in <sc_logic>	XSR12_PIN_D9;
	sc_in <sc_logic>	XSR12_PIN_D10;
	sc_in <sc_logic>	XSR12_PIN_D11;
	sc_out <sc_logic>	XSR12_PIN_Q0;
	sc_out <sc_logic>	XSR12_PIN_Q1;
	sc_out <sc_logic>	XSR12_PIN_Q2;
	sc_out <sc_logic>	XSR12_PIN_Q3;
	sc_out <sc_logic>	XSR12_PIN_Q4;
	sc_out <sc_logic>	XSR12_PIN_Q5;
	sc_out <sc_logic>	XSR12_PIN_Q6;
	sc_out <sc_logic>	XSR12_PIN_Q7;
	sc_out <sc_logic>	XSR12_PIN_Q8;
	sc_out <sc_logic>	XSR12_PIN_Q9;
	sc_out <sc_logic>	XSR12_PIN_Q10;
	sc_out <sc_logic>	XSR12_PIN_Q11;

	SC_HAS_PROCESS(SCM_XSR12);

	SCM_XSR12(sc_module_name nm, const char *arg);

	private:
	struct scm_xsr12_state *state;
	void doit(void);
};

#endif /* R1000_XSR12 */
