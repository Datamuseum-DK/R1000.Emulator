#ifndef R1000_LS125
#define R1000_LS125

struct scm_LS125_state;

SC_MODULE(SCM_LS125)
{
	sc_in <sc_logic>	pin1;	// E (input)
	sc_in <sc_logic>	pin2;	// D (input)
	sc_inout_resolved	pin3;	// Q (tri_state)

	SC_HAS_PROCESS(SCM_LS125);

	SCM_LS125(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2;
		loadit(arg);
	}

	private:
	struct scm_LS125_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_LS125 */
