#ifndef R1000_2167
#define R1000_2167

// CMOS Static RAM 16K x 1-Bit
// Rensas DSC2981/08 February 2001


struct scm_2167_state;

SC_MODULE(SCM_2167)
{
	sc_in <sc_logic>	pin1;	// A0 (input)
	sc_in <sc_logic>	pin2;	// A1 (input)
	sc_in <sc_logic>	pin3;	// A2 (input)
	sc_in <sc_logic>	pin4;	// A3 (input)
	sc_in <sc_logic>	pin5;	// A4 (input)
	sc_in <sc_logic>	pin6;	// A5 (input)
	sc_in <sc_logic>	pin7;	// A6 (input)
	sc_out_resolved		pin8;	// Q (output)
	sc_in <sc_logic>	pin9;	// WE_ (input)
	sc_in <sc_logic>	pin11;	// CS_ (input)
	sc_in <sc_logic>	pin12;	// D (input)
	sc_in <sc_logic>	pin13;	// A7 (input)
	sc_in <sc_logic>	pin14;	// A8 (input)
	sc_in <sc_logic>	pin15;	// A9 (input)
	sc_in <sc_logic>	pin16;	// A10 (input)
	sc_in <sc_logic>	pin17;	// A11 (input)
	sc_in <sc_logic>	pin18;	// A12 (input)
	sc_in <sc_logic>	pin19;	// A13 (input)

	SC_HAS_PROCESS(SCM_2167);

	SCM_2167(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin6 << pin7
		    << pin9 << pin11 << pin12
		    << pin13 << pin14 << pin15 << pin16 << pin17 << pin18 << pin19;
		loadit(arg);
	}

	private:
	struct scm_2167_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_2167 */
