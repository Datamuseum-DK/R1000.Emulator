#ifndef R1000_XREG32
#define R1000_XREG32

#include "XREG32_pins.hh"

struct scm_xreg32_state;

SC_MODULE(SCM_XREG32)
{

	XREG32_PINLIST

	SC_HAS_PROCESS(SCM_XREG32);

	SCM_XREG32(sc_module_name nm, const char *arg);

	private:
	struct scm_xreg32_state *state;
	void doit(void);
};

#endif /* R1000_XREG32 */
