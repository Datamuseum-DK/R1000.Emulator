// 16 bit two input multiplexer

#ifndef R1000_XMUX16
#define R1000_XMUX16

#include "XMUX16_pins.hh"

struct scm_xmux16_state;

SC_MODULE(SCM_XMUX16)
{

	XMUX16_PINLIST

	SC_HAS_PROCESS(SCM_XMUX16);

	SCM_XMUX16(sc_module_name nm, const char *arg);

	private:
	struct scm_xmux16_state *state;
	void doit(void);
};

#endif /* R1000_XMUX16 */
