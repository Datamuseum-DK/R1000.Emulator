// 8 bit adder

#ifndef R1000_XADD8
#define R1000_XADD8

#include "XADD8_pins.hh"

struct scm_xadd8_state;

SC_MODULE(SCM_XADD8)
{

	XADD8_PINLIST

	SC_HAS_PROCESS(SCM_XADD8);

	SCM_XADD8(sc_module_name nm, const char *arg);

	private:
	struct scm_xadd8_state *state;
	void doit(void);
};

#endif /* R1000_XADD8 */
