#ifndef R1000_F08
#define R1000_F08

// (Quad) 2-input AND


struct scm_f08_state;

SC_MODULE(SCM_F08)
{
	sc_in <sc_logic>	pin1;	// D0 (input)
	sc_in <sc_logic>	pin2;	// D1 (input)
	sc_out <sc_logic>	pin3;	// Q (output)

	SC_HAS_PROCESS(SCM_F08);

	SCM_F08(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2;
		loadit(arg);
	}

	private:
	struct scm_f08_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F08 */
