#ifndef R1000_XEQ40
#define R1000_XEQ40

#include "XEQ40_pins.hh"

struct scm_xeq40_state;

SC_MODULE(SCM_XEQ40)
{

	XEQ40_PINLIST

	SC_HAS_PROCESS(SCM_XEQ40);

	SCM_XEQ40(sc_module_name nm, const char *arg);

	private:
	struct scm_xeq40_state *state;
	void doit(void);
};

#endif /* R1000_XEQ40 */
