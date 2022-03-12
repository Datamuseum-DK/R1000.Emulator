#ifndef R1000_F251
#define R1000_F251

#include "F251_pins.hh"

// 8-Input Multiplexer with 3-STATE Outputs
// Fairchild DS009504 April 1988 Revised September 2000


struct scm_f251_state;

SC_MODULE(SCM_F251)
{
	sc_in <sc_logic>	F251_PIN_OE_;
	sc_in <sc_logic>	F251_PIN_S0;
	sc_in <sc_logic>	F251_PIN_S1;
	sc_in <sc_logic>	F251_PIN_S2;
	sc_out_resolved		F251_PIN_Y;
	sc_out_resolved		F251_PIN_Y_;
	sc_in <sc_logic>	F251_PIN_A;
	sc_in <sc_logic>	F251_PIN_B;
	sc_in <sc_logic>	F251_PIN_C;
	sc_in <sc_logic>	F251_PIN_D;
	sc_in <sc_logic>	F251_PIN_E;
	sc_in <sc_logic>	F251_PIN_F;
	sc_in <sc_logic>	F251_PIN_G;
	sc_in <sc_logic>	F251_PIN_H;

	SC_HAS_PROCESS(SCM_F251);

	SCM_F251(sc_module_name nm, const char *arg);

	private:
	struct scm_f251_state *state;
	void doit(void);
};

#endif /* R1000_F251 */
