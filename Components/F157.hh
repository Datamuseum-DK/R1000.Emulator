#ifndef R1000_F157
#define R1000_F157

// Quad 2-input data selector/multiplexer, inverting
// Philips IC15 1996 Mar 12


struct scm_f157_state;

SC_MODULE(SCM_F157)
{
	sc_in <sc_logic>	pin1;	// S (input)
	sc_in <sc_logic>	pin2;	// A0 (input)
	sc_in <sc_logic>	pin3;	// B0 (input)
	sc_out <sc_logic>	pin4;	// Y0_ (output)
	sc_in <sc_logic>	pin5;	// A1 (input)
	sc_in <sc_logic>	pin6;	// B1 (input)
	sc_out <sc_logic>	pin7;	// Y1_ (output)
	sc_out <sc_logic>	pin9;	// Y2_ (output)
	sc_in <sc_logic>	pin10;	// B2 (input)
	sc_in <sc_logic>	pin11;	// A2 (input)
	sc_out <sc_logic>	pin12;	// Y3_ (output)
	sc_in <sc_logic>	pin13;	// B3 (input)
	sc_in <sc_logic>	pin14;	// A3 (input)
	sc_in <sc_logic>	pin15;	// E_ (input)

	SC_HAS_PROCESS(SCM_F157);

	SCM_F157(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3 << pin5 << pin6
			  << pin10 << pin11 << pin13 << pin14 << pin15;
		loadit(arg);
	}

	private:
	struct scm_f157_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_F157 */
