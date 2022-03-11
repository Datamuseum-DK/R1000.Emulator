#ifndef R1000_XPAXXXL
#define R1000_XPAXXXL

#include "XPAXXXL_pins.hh"

struct scm_xpaxxxl_state;

SC_MODULE(SCM_XPAXXXL)
{
	sc_in <sc_logic>	XPAXXXL_PIN_CLK;
	sc_in <sc_logic>	XPAXXXL_PIN_A0;
	sc_in <sc_logic>	XPAXXXL_PIN_A1;
	sc_in <sc_logic>	XPAXXXL_PIN_A2;
	sc_in <sc_logic>	XPAXXXL_PIN_A3;
	sc_in <sc_logic>	XPAXXXL_PIN_A4;
	sc_in <sc_logic>	XPAXXXL_PIN_A5;
	sc_in <sc_logic>	XPAXXXL_PIN_A6;
	sc_in <sc_logic>	XPAXXXL_PIN_A7;
	sc_in <sc_logic>	XPAXXXL_PIN_A8;
	sc_out <sc_logic>	XPAXXXL_PIN_Y0;
	sc_out <sc_logic>	XPAXXXL_PIN_Y1;
	sc_out <sc_logic>	XPAXXXL_PIN_Y2;
	sc_out <sc_logic>	XPAXXXL_PIN_Y3;
	sc_out <sc_logic>	XPAXXXL_PIN_Y4;
	sc_out <sc_logic>	XPAXXXL_PIN_Y5;
	sc_out <sc_logic>	XPAXXXL_PIN_Y6;
	sc_out <sc_logic>	XPAXXXL_PIN_Y7;

	SC_HAS_PROCESS(SCM_XPAXXXL);

	SCM_XPAXXXL(sc_module_name nm, const char *arg);

	private:
	struct scm_xpaxxxl_state *state;
	void doit(void);
	uint8_t prom[512];
};

#endif /* R1000_XPAXXXL */
