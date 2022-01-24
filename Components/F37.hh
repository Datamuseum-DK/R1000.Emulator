#ifndef R1000_F37
#define R1000_F37

// (Quad) Two-Input NAND Buffer


struct scm_f37_state;

SC_MODULE(SCM_F37)
{
	sc_in <sc_logic>	pin1;	// D0 (input)
	sc_in <sc_logic>	pin2;	// D1 (input)
	sc_out <sc_logic>	pin3;	// Q (output)

	SC_HAS_PROCESS(SCM_F37);

	SCM_F37(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_THREAD(doit);
		loadit(arg);
	}

	private:
	struct scm_f37_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F37 */
