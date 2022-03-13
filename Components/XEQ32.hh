#ifndef R1000_XEQ32
#define R1000_XEQ32

#include "XEQ32_pins.hh"

struct scm_xeq32_state;

SC_MODULE(SCM_XEQ32)
{

	XEQ32_PINLIST

	SC_HAS_PROCESS(SCM_XEQ32);

	SCM_XEQ32(sc_module_name nm, const char *arg);

	private:
	struct scm_xeq32_state *state;
	void doit(void);
};

#endif /* R1000_XEQ32 */
