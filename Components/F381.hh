#ifndef R1000_F381
#define R1000_F381


struct scm_f381_state;

SC_MODULE(SCM_F381)
{
	sc_in <sc_logic>	pin1;	// A2 (input)
	sc_in <sc_logic>	pin2;	// D2 (input)
	sc_in <sc_logic>	pin3;	// A3 (input)
	sc_in <sc_logic>	pin4;	// D3 (input)
	sc_in <sc_logic>	pin5;	// S2 (input)
	sc_in <sc_logic>	pin6;	// S1 (input)
	sc_in <sc_logic>	pin7;	// S0 (input)
	sc_out <sc_logic>	pin8;	// F3 (output)
	sc_out <sc_logic>	pin9;	// F2 (output)
	sc_out <sc_logic>	pin11;	// F1 (output)
	sc_out <sc_logic>	pin12;	// F0 (output)
	sc_out <sc_logic>	pin13;	// G_ (output)
	sc_out <sc_logic>	pin14;	// P_ (output)
	sc_in <sc_logic>	pin15;	// CI (input)
	sc_in <sc_logic>	pin16;	// D0 (input)
	sc_in <sc_logic>	pin17;	// A0 (input)
	sc_in <sc_logic>	pin18;	// D1 (input)
	sc_in <sc_logic>	pin19;	// A1 (input)

	SC_HAS_PROCESS(SCM_F381);

	SCM_F381(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin6
		    << pin7 << pin15 << pin16 << pin17 << pin18 << pin19;
		loadit(arg);
	}

	private:
	struct scm_f381_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F381 */
