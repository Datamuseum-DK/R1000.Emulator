#ifndef R1000_F299
#define R1000_F299

#include "Components/F299_pins.hh"

// Octal Universal Shift/Storage Register with Common Parallel I/O Pins
// Fairchild DS009515 April 1988 Revised September 2000

struct scm_f299_state;

SC_MODULE(SCM_F299)
{
	sc_in <sc_logic>	F299_PIN_S0;
	sc_in <sc_logic>	F299_PIN_S1;
	sc_in <sc_logic>	F299_PIN_G1;
	sc_in <sc_logic>	F299_PIN_RSI;
	sc_in <sc_logic>	F299_PIN_LSI;
	sc_in <sc_logic>	F299_PIN_CLK;
	sc_in <sc_logic>	F299_PIN_CLR;

	sc_in <sc_logic>	F299_PIN_G2;
	sc_out <sc_logic>	F299_PIN_Q0;
	sc_inout_resolved	F299_PIN_DQ0;
	sc_inout_resolved	F299_PIN_DQ1;
	sc_inout_resolved	F299_PIN_DQ2;
	sc_inout_resolved	F299_PIN_DQ3;
	sc_inout_resolved	F299_PIN_DQ4;
	sc_inout_resolved	F299_PIN_DQ5;
	sc_inout_resolved	F299_PIN_DQ6;
	sc_inout_resolved	F299_PIN_DQ7;
	sc_out <sc_logic>	F299_PIN_Q7;

	SC_HAS_PROCESS(SCM_F299);

	SCM_F299(sc_module_name nm, const char *arg);

	private:
	struct scm_f299_state *state;
	void doit(void);
};

#endif /* R1000_F299 */
