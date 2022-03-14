#ifndef R1000_XBUF20
#define R1000_XBUF20

#include "XBUF20_pins.hh"

struct scm_xbuf20_state;

SC_MODULE(SCM_XBUF20)
{

	XBUF20_PINLIST

	SC_HAS_PROCESS(SCM_XBUF20);

	SCM_XBUF20(sc_module_name nm, const char *arg);

	private:
	struct scm_xbuf20_state *state;
	void doit(void);
};

#endif /* R1000_XBUF20 */
