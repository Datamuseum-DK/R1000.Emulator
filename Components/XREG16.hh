#ifndef R1000_XREG16
#define R1000_XREG16

#include "XREG16_pins.hh"

struct scm_xreg16_state;

SC_MODULE(SCM_XREG16)
{

	XREG16_PINLIST

	SC_HAS_PROCESS(SCM_XREG16);

	SCM_XREG16(sc_module_name nm, const char *arg);

	private:
	struct scm_xreg16_state *state;
	void doit(void);
};

#endif /* R1000_XREG16 */
