#ifndef R1000_PHITPAL
#define R1000_PHITPAL

struct scm_phitpal_state;

SC_MODULE(SCM_PHITPAL)
{
	sc_in <sc_logic>	pin1;	// I0 (input)
	sc_in <sc_logic>	pin2;	// I1 (input)
	sc_in <sc_logic>	pin3;	// I2 (input)
	sc_in <sc_logic>	pin4;	// I3 (input)
	sc_in <sc_logic>	pin5;	// I4 (input)
	sc_in <sc_logic>	pin6;	// I5 (input)
	sc_in <sc_logic>	pin7;	// I6 (input)
	sc_in <sc_logic>	pin8;	// I7 (input)
	sc_in <sc_logic>	pin9;	// I8 (input)
	sc_in <sc_logic>	pin11;	// I9 (input)
	sc_out <sc_logic>	pin12;	// O1 (output)
	sc_out <sc_logic>	pin13;	// D5 (output)
	sc_out <sc_logic>	pin14;	// D4 (output)
	sc_out <sc_logic>	pin15;	// D3 (output)
	sc_out <sc_logic>	pin16;	// D2 (output)
	sc_out <sc_logic>	pin17;	// D1 (output)
	sc_out <sc_logic>	pin18;	// D0 (output)
	sc_out <sc_logic>	pin19;	// O0 (output)

	SC_HAS_PROCESS(SCM_PHITPAL);

	SCM_PHITPAL(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin6
		    << pin7 << pin8 << pin9 << pin11;
		loadit(arg);
	}

	private:
	struct scm_phitpal_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_PHITPAL */
