#ifndef R1000_XEQ16
#define R1000_XEQ16

#include "XEQ16_pins.hh"

struct scm_xeq16_state;

SC_MODULE(SCM_XEQ16)
{

	XEQ16_PINLIST

	SC_HAS_PROCESS(SCM_XEQ16);

	SCM_XEQ16(sc_module_name nm, const char *arg);

	private:
	struct scm_xeq16_state *state;
	void doit(void);
};

#endif /* R1000_XEQ16 */
