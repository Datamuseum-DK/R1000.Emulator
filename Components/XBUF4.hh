#ifndef R1000_XBUF4
#define R1000_XBUF4

#include "XBUF4_pins.hh"

struct scm_xbuf4_state;

SC_MODULE(SCM_XBUF4)
{

	XBUF4_PINLIST

	SC_HAS_PROCESS(SCM_XBUF4);

	SCM_XBUF4(sc_module_name nm, const char *arg);

	private:
	struct scm_xbuf4_state *state;
	void doit(void);
};

#endif /* R1000_XBUF4 */
