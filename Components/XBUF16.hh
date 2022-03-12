#ifndef R1000_XBUF16
#define R1000_XBUF16

#include "XBUF16_pins.hh"

struct scm_xbuf16_state;

SC_MODULE(SCM_XBUF16)
{

	XBUF16_PINLIST

	SC_HAS_PROCESS(SCM_XBUF16);

	SCM_XBUF16(sc_module_name nm, const char *arg);

	private:
	struct scm_xbuf16_state *state;
	void doit(void);
};

#endif /* R1000_XBUF16 */
