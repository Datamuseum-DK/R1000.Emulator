#ifndef R1000_F194
#define R1000_F194

#include "F194_pins.hh"

// 4-bit bidirectional universal shift register
// Philips 1989 Apr 04, IC15 Data Handbook

struct scm_f194_state;

SC_MODULE(SCM_F194)
{
	sc_in <sc_logic>	F194_PIN_CLK;
	sc_in <sc_logic>	F194_PIN_CLR;
	sc_in <sc_logic>	F194_PIN_RSI;
	sc_in <sc_logic>	F194_PIN_LSI;
	sc_in <sc_logic>	F194_PIN_S0;
	sc_in <sc_logic>	F194_PIN_S1;
	sc_in <sc_logic>	F194_PIN_D0;
	sc_in <sc_logic>	F194_PIN_D1;
	sc_in <sc_logic>	F194_PIN_D2;
	sc_in <sc_logic>	F194_PIN_D3;
	sc_out <sc_logic>	F194_PIN_Q0;
	sc_out <sc_logic>	F194_PIN_Q1;
	sc_out <sc_logic>	F194_PIN_Q2;
	sc_out <sc_logic>	F194_PIN_Q3;

	SC_HAS_PROCESS(SCM_F194);

	SCM_F194(sc_module_name nm, const char *arg);

	private:
	struct scm_f194_state *state;
	void doit(void);
};

#endif /* R1000_F194 */
