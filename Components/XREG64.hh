#ifndef R1000_XREG64
#define R1000_XREG64

#include "XREG64_pins.hh"

struct scm_xreg64_state;

SC_MODULE(SCM_XREG64)
{

	XREG64_PINLIST

	SC_HAS_PROCESS(SCM_XREG64);

	SCM_XREG64(sc_module_name nm, const char *arg);

	private:
	struct scm_xreg64_state *state;
	void doit(void);
};

#endif /* R1000_XREG64 */
