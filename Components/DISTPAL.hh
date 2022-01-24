#ifndef R1000_DISTPAL
#define R1000_DISTPAL

struct scm_distpal_state;

SC_MODULE(SCM_DISTPAL)
{
	sc_in <sc_logic>	pin1;	// CLK (input)
	sc_in <sc_logic>	pin2;	// I0 (input)
	sc_in <sc_logic>	pin3;	// I1 (input)
	sc_in <sc_logic>	pin4;	// I2 (input)
	sc_in <sc_logic>	pin5;	// I3 (input)
	sc_in <sc_logic>	pin6;	// I4 (input)
	sc_in <sc_logic>	pin7;	// I5 (input)
	sc_in <sc_logic>	pin8;	// I6 (input)
	sc_in <sc_logic>	pin9;	// I7 (input)
	sc_in <sc_logic>	pin11;	// OE_ (input)
	sc_out <sc_logic>	pin16;	// Q3 (output)
	sc_out <sc_logic>	pin17;	// Q2 (output)
	sc_out <sc_logic>	pin18;	// Q1 (output)
	sc_out <sc_logic>	pin19;	// Q0 (output)

	SC_HAS_PROCESS(SCM_DISTPAL);

	SCM_DISTPAL(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1.pos() << pin11;
		loadit(arg);
	}

	private:
	struct scm_distpal_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_DISTPAL */