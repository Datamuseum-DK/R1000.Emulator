#ifndef R1000_F00
#define R1000_F00

// (Quad) 2-input NAND


struct scm_f00_state;

SC_MODULE(SCM_F00)
{
	sc_in <sc_logic>	pin1;	// D0 (input)
	sc_in <sc_logic>	pin2;	// D1 (input)
	sc_out <sc_logic>	pin3;	// Q (output)

	SC_HAS_PROCESS(SCM_F00);

	SCM_F00(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2;
		loadit(arg);
	}

	private:
	struct scm_f00_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F00 */
