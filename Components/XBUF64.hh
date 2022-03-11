#ifndef R1000_XBUF64
#define R1000_XBUF64

#include "XBUF64_pins.hh"

struct scm_xbuf64_state;

SC_MODULE(SCM_XBUF64)
{

	XBUF64_PINLIST

	SC_HAS_PROCESS(SCM_XBUF64);

	SCM_XBUF64(sc_module_name nm, const char *arg);

	private:
	struct scm_xbuf64_state *state;
	void doit(void);
};

#endif /* R1000_XBUF64 */
