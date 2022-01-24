#ifndef R1000_F30
#define R1000_F30


struct scm_f30_state;

SC_MODULE(SCM_F30)
{
	sc_in <sc_logic>	pin1;	// D0 (input)
	sc_in <sc_logic>	pin2;	// D3 (input)
	sc_in <sc_logic>	pin3;	// D4 (input)
	sc_in <sc_logic>	pin4;	// D5 (input)
	sc_in <sc_logic>	pin5;	// D6 (input)
	sc_in <sc_logic>	pin6;	// D7 (input)
	sc_out <sc_logic>	pin8;	// Q (output)
	sc_in <sc_logic>	pin11;	// D1 (input)
	sc_in <sc_logic>	pin12;	// D2 (input)

	SC_HAS_PROCESS(SCM_F30);

	SCM_F30(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3 << pin4
			  << pin5 << pin6 << pin11 << pin12;
		loadit(arg);
	}

	private:
	struct scm_f30_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F30 */
