#ifndef R1000_XBUF8
#define R1000_XBUF8

#include "XBUF8_pins.hh"

struct scm_xbuf8_state;

SC_MODULE(SCM_XBUF8)
{

	XBUF8_PINLIST

	SC_HAS_PROCESS(SCM_XBUF8);

	SCM_XBUF8(sc_module_name nm, const char *arg);

	private:
	struct scm_xbuf8_state *state;
	void doit(void);
};

#endif /* R1000_XBUF8 */
