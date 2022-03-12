#ifndef R1000_XWCSRAM
#define R1000_XWCSRAM

#include "XWCSRAM_pins.hh"

struct scm_xwcsram_state;

SC_MODULE(SCM_XWCSRAM)
{
	sc_in <sc_logic>	XWCSRAM_PIN_WE;
	sc_in <sc_logic>	XWCSRAM_PIN_A0;
	sc_in <sc_logic>	XWCSRAM_PIN_A1;
	sc_in <sc_logic>	XWCSRAM_PIN_A2;
	sc_in <sc_logic>	XWCSRAM_PIN_A3;
	sc_in <sc_logic>	XWCSRAM_PIN_A4;
	sc_in <sc_logic>	XWCSRAM_PIN_A5;
	sc_in <sc_logic>	XWCSRAM_PIN_A6;
	sc_in <sc_logic>	XWCSRAM_PIN_A7;
	sc_in <sc_logic>	XWCSRAM_PIN_A8;
	sc_in <sc_logic>	XWCSRAM_PIN_A9;
	sc_in <sc_logic>	XWCSRAM_PIN_A10;
	sc_in <sc_logic>	XWCSRAM_PIN_A11;
	sc_in <sc_logic>	XWCSRAM_PIN_A12;
	sc_in <sc_logic>	XWCSRAM_PIN_A13;
	sc_in <sc_logic>	XWCSRAM_PIN_D0;
	sc_in <sc_logic>	XWCSRAM_PIN_D1;
	sc_in <sc_logic>	XWCSRAM_PIN_D2;
	sc_in <sc_logic>	XWCSRAM_PIN_D3;
	sc_in <sc_logic>	XWCSRAM_PIN_D4;
	sc_in <sc_logic>	XWCSRAM_PIN_D5;
	sc_in <sc_logic>	XWCSRAM_PIN_D6;
	sc_in <sc_logic>	XWCSRAM_PIN_D7;
	sc_out <sc_logic>	XWCSRAM_PIN_Q0;
	sc_out <sc_logic>	XWCSRAM_PIN_Q1;
	sc_out <sc_logic>	XWCSRAM_PIN_Q2;
	sc_out <sc_logic>	XWCSRAM_PIN_Q3;
	sc_out <sc_logic>	XWCSRAM_PIN_Q4;
	sc_out <sc_logic>	XWCSRAM_PIN_Q5;
	sc_out <sc_logic>	XWCSRAM_PIN_Q6;
	sc_out <sc_logic>	XWCSRAM_PIN_Q7;

	SC_HAS_PROCESS(SCM_XWCSRAM);

	SCM_XWCSRAM(sc_module_name nm, const char *arg);

	private:
	struct scm_xwcsram_state *state;
	void doit(void);
};

#endif /* R1000_XWCSRAM */
