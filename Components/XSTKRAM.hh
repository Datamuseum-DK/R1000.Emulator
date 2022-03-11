#ifndef R1000_XSTKRAM
#define R1000_XSTKRAM

#include "Components/XSTKRAM_pins.hh"

struct scm_xstkram_state;

#define XSTKRAM_PINS() \
	PIN(0, XSTKRAM_PIN_D0, XSTKRAM_PIN_Q0) \
	PIN(1, XSTKRAM_PIN_D1, XSTKRAM_PIN_Q1) \
	PIN(2, XSTKRAM_PIN_D2, XSTKRAM_PIN_Q2) \
	PIN(3, XSTKRAM_PIN_D3, XSTKRAM_PIN_Q3) \
	PIN(4, XSTKRAM_PIN_D4, XSTKRAM_PIN_Q4) \
	PIN(5, XSTKRAM_PIN_D5, XSTKRAM_PIN_Q5) \
	PIN(6, XSTKRAM_PIN_D6, XSTKRAM_PIN_Q6) \
	PIN(7, XSTKRAM_PIN_D7, XSTKRAM_PIN_Q7) \
	PIN(8, XSTKRAM_PIN_D8, XSTKRAM_PIN_Q8) \
	PIN(9, XSTKRAM_PIN_D9, XSTKRAM_PIN_Q9) \
	PIN(10, XSTKRAM_PIN_D10, XSTKRAM_PIN_Q10) \
	PIN(11, XSTKRAM_PIN_D11, XSTKRAM_PIN_Q11) \
	PIN(12, XSTKRAM_PIN_D12, XSTKRAM_PIN_Q12) \
	PIN(13, XSTKRAM_PIN_D13, XSTKRAM_PIN_Q13) \
	PIN(14, XSTKRAM_PIN_D14, XSTKRAM_PIN_Q14) \
	PIN(15, XSTKRAM_PIN_D15, XSTKRAM_PIN_Q15)


SC_MODULE(SCM_XSTKRAM)
{

	#define PIN(bit, pin_in, pin_out) \
	sc_in <sc_logic>	pin_in;
	XSTKRAM_PINS()
	#undef PIN

	#define PIN(bit, pin_in, pin_out) \
	sc_out <sc_logic>	pin_out;
	XSTKRAM_PINS()
	#undef PIN

	sc_in <sc_logic>	XSTKRAM_PIN_WE;
	sc_in <sc_logic>	XSTKRAM_PIN_CS;
	sc_in <sc_logic>	XSTKRAM_PIN_A0;
	sc_in <sc_logic>	XSTKRAM_PIN_A1;
	sc_in <sc_logic>	XSTKRAM_PIN_A2;
	sc_in <sc_logic>	XSTKRAM_PIN_A3;

	SC_HAS_PROCESS(SCM_XSTKRAM);

	SCM_XSTKRAM(sc_module_name nm, const char *arg);

	private:
	struct scm_xstkram_state *state;
	void doit(void);
};

#endif /* R1000_XSTKRAM */
