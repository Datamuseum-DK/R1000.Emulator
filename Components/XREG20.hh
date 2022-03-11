#ifndef R1000_XREG20
#define R1000_XREG20

#include "XREG20_pins.hh"

struct scm_xreg20_state;

SC_MODULE(SCM_XREG20)
{

	XREG20_PINLIST

	SC_HAS_PROCESS(SCM_XREG20);

	SCM_XREG20(sc_module_name nm, const char *arg);

	private:
	struct scm_xreg20_state *state;
	void doit(void);
};

#endif /* R1000_XREG20 */
