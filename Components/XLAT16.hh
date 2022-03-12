#ifndef R1000_XLAT16
#define R1000_XLAT16

#include "XLAT16_pins.hh"

struct scm_xlat16_state;

SC_MODULE(SCM_XLAT16)
{

	XLAT16_PINLIST

	SC_HAS_PROCESS(SCM_XLAT16);

	SCM_XLAT16(sc_module_name nm, const char *arg);

	private:
	struct scm_xlat16_state *state;
	void doit(void);
};

#endif /* R1000_XLAT16 */
