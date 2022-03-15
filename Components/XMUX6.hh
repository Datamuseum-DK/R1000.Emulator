// 6 bit two input multiplexer

#ifndef R1000_XMUX6
#define R1000_XMUX6

#include "XMUX6_pins.hh"

struct scm_xmux6_state;

SC_MODULE(SCM_XMUX6)
{

	XMUX6_PINLIST

	SC_HAS_PROCESS(SCM_XMUX6);

	SCM_XMUX6(sc_module_name nm, const char *arg);

	private:
	struct scm_xmux6_state *state;
	void doit(void);
};

#endif /* R1000_XMUX6 */
