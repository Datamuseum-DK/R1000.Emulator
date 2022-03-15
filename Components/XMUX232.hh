#ifndef R1000_XMUX232
#define R1000_XMUX232

#include "XMUX232_pins.hh"

struct scm_xmux232_state;

SC_MODULE(SCM_XMUX232)
{

	XMUX232_PINLIST

	SC_HAS_PROCESS(SCM_XMUX232);

	SCM_XMUX232(sc_module_name nm, const char *arg);

	private:
	struct scm_xmux232_state *state;
};

#endif /* R1000_XMUX232 */
