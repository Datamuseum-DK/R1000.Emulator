#ifndef R1000_SETPAL
#define R1000_SETPAL

struct scm_setpal_state;

SC_MODULE(SCM_SETPAL)
{
	sc_in <sc_logic>	pin1;	// I0/CLK (input)
	sc_in <sc_logic>	pin2;	// I1 (input)
	sc_in <sc_logic>	pin3;	// I2 (input)
	sc_in <sc_logic>	pin4;	// I3 (input)
	sc_in <sc_logic>	pin5;	// I4 (input)
	sc_in <sc_logic>	pin6;	// I5 (input)
	sc_in <sc_logic>	pin7;	// I6 (input)
	sc_in <sc_logic>	pin8;	// I7 (input)
	sc_in <sc_logic>	pin9;	// I8 (input)
	sc_in <sc_logic>	pin10;	// I9 (input)
	sc_in <sc_logic>	pin11;	// I10 (input)
	sc_in <sc_logic>	pin13;	// I11 (input)
	sc_in <sc_logic>	pin14;	// D9 (input)
	sc_in <sc_logic>	pin15;	// D8 (input)
	sc_out <sc_logic>	pin16;	// D7 (output)
	sc_out <sc_logic>	pin17;	// D6 (output)
	sc_out <sc_logic>	pin18;	// D5 (output)
	sc_out <sc_logic>	pin19;	// D4 (output)
	sc_out <sc_logic>	pin20;	// D3 (output)
	sc_out <sc_logic>	pin21;	// D2 (output)
	sc_out <sc_logic>	pin22;	// D1 (output)
	sc_out <sc_logic>	pin23;	// D0 (output)

	SC_HAS_PROCESS(SCM_SETPAL);

	SCM_SETPAL(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1.pos() << pin8 << pin10;
		loadit(arg);
	}

	private:
	struct scm_setpal_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_SETPAL */
