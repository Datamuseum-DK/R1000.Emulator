#ifndef R1000_XEQ20
#define R1000_XEQ20

#include "XEQ20_pins.hh"

struct scm_xeq20_state;

SC_MODULE(SCM_XEQ20)
{

	XEQ20_PINLIST

	SC_HAS_PROCESS(SCM_XEQ20);

	SCM_XEQ20(sc_module_name nm, const char *arg);

	private:
	struct scm_xeq20_state *state;
	void doit(void);
};

#endif /* R1000_XEQ20 */
