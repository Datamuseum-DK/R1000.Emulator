#ifndef R1000_F283
#define R1000_F283

// 4-Bit Binary Full Adder with Fast Carry
// Fairchild DS009513 April 1988 Revised January 2004


struct scm_f283_state;

SC_MODULE(SCM_F283)
{
	sc_out <sc_logic>	pin1;	// S1 - Y2 (output)
	sc_in <sc_logic>	pin2;	// B1 - B2 (input)
	sc_in <sc_logic>	pin3;	// A1 - A2 (input)
	sc_out <sc_logic>	pin4;	// S0 - Y3 (output)
	sc_in <sc_logic>	pin5;	// A0 - A3 (input)
	sc_in <sc_logic>	pin6;	// B0 - B3 (input)
	sc_in <sc_logic>	pin7;	// C0 - CI (input)
	sc_out <sc_logic>	pin9;	// C4 - C0 (output)
	sc_out <sc_logic>	pin10;	// S3 - Y0 (output)
	sc_in <sc_logic>	pin11;	// B3 - B0 (input)
	sc_in <sc_logic>	pin12;	// A3 - A0 (input)
	sc_out <sc_logic>	pin13;	// S2 - Y1 (output)
	sc_in <sc_logic>	pin14;	// A2 - A1 (input)
	sc_in <sc_logic>	pin15;	// B2 - B1 (input)

	SC_HAS_PROCESS(SCM_F283);

	SCM_F283(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin2 << pin3 << pin5 << pin6 << pin7 << pin11
			  << pin12 << pin14 << pin15;
		loadit(arg);
	}

	private:
	struct scm_f283_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F283 */
