#ifndef R1000_XBUF32
#define R1000_XBUF32

#include "XBUF32_pins.hh"

struct scm_xbuf32_state;

SC_MODULE(SCM_XBUF32)
{

	XBUF32_PINLIST

	SC_HAS_PROCESS(SCM_XBUF32);

	SCM_XBUF32(sc_module_name nm, const char *arg);

	private:
	struct scm_xbuf32_state *state;
	void doit(void);
};

#endif /* R1000_XBUF32 */
