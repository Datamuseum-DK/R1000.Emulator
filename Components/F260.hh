#ifndef R1000_F260
#define R1000_F260

// (Dual) 5-input NOR gate
// Philips IC15 1988 Nov 29


struct scm_f260_state;

SC_MODULE(SCM_F260)
{
	sc_in <sc_logic>	pin1;	// D2 (input)
	sc_in <sc_logic>	pin2;	// D3 (input)
	sc_in <sc_logic>	pin3;	// D4 (input)
	sc_out <sc_logic>	pin5;	// Q (output)
	sc_in <sc_logic>	pin12;	// D0 (input)
	sc_in <sc_logic>	pin13;	// D1 (input)

	SC_HAS_PROCESS(SCM_F260);

	SCM_F260(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3 << pin12 << pin13;
		loadit(arg);
	}

	private:
	struct scm_f260_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F260 */
