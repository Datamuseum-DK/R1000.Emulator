#ifndef R1000_XLAT64
#define R1000_XLAT64

#include "XLAT64_pins.hh"

struct scm_xlat64_state;

SC_MODULE(SCM_XLAT64)
{

	XLAT64_PINLIST

	SC_HAS_PROCESS(SCM_XLAT64);

	SCM_XLAT64(sc_module_name nm, const char *arg);

	private:
	struct scm_xlat64_state *state;
	void doit(void);
};

#endif /* R1000_XLAT64 */
