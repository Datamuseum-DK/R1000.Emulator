#ifndef R1000_XBUF48
#define R1000_XBUF48

#include "XBUF48_pins.hh"

struct scm_xbuf48_state;

SC_MODULE(SCM_XBUF48)
{

	XBUF48_PINLIST

	SC_HAS_PROCESS(SCM_XBUF48);

	SCM_XBUF48(sc_module_name nm, const char *arg);

	private:
	struct scm_xbuf48_state *state;
	void doit(void);
};

#endif /* R1000_XBUF48 */
