#ifndef R1000_F38
#define R1000_F38

// (Quad) Two-Input NAND Buffer (Open Collector)
// Fairchild DS009465 April 1988 Revised September 2000


struct scm_f38_state;

SC_MODULE(SCM_F38)
{
	sc_in <sc_logic>	pin1;	// D0 (input)
	sc_in <sc_logic>	pin2;	// D1 (input)
	sc_out <sc_logic>	pin3;	// Q (output)

	SC_HAS_PROCESS(SCM_F38);

	SCM_F38(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2;
		loadit(arg);
	}

	private:
	struct scm_f38_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F38 */
