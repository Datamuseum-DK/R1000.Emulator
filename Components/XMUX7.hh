// 7 bit two input multiplexer

#ifndef R1000_XMUX7
#define R1000_XMUX7

#include "XMUX7_pins.hh"

struct scm_xmux7_state;

SC_MODULE(SCM_XMUX7)
{

	XMUX7_PINLIST

	SC_HAS_PROCESS(SCM_XMUX7);

	SCM_XMUX7(sc_module_name nm, const char *arg);

	private:
	struct scm_xmux7_state *state;
	void doit(void);
};

#endif /* R1000_XMUX7 */
