#ifndef R1000_F40
#define R1000_F40

// (Dual) 4-input NAND buffer
// Philips 853-0053 96314 April 11, 1989


struct scm_f40_state;

SC_MODULE(SCM_F40)
{
	sc_in <sc_logic>	pin1;	// D0 (input)
	sc_in <sc_logic>	pin2;	// D3 (input)
	sc_in <sc_logic>	pin4;	// D4 (input)
	sc_in <sc_logic>	pin5;	// D5 (input)
	sc_out <sc_logic>	pin6;	// Q (output)

	SC_HAS_PROCESS(SCM_F40);

	SCM_F40(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin4 << pin5;
		loadit(arg);
	}

	private:
	struct scm_f40_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F40 */
