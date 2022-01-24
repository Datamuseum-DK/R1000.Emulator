#ifndef R1000_F299
#define R1000_F299

// Octal Universal Shift/Storage Register with Common Parallel I/O Pins
// Fairchild DS009515 April 1988 Revised September 2000


struct scm_f299_state;

SC_MODULE(SCM_F299)
{
	sc_in <sc_logic>	pin1;	// S0 - S1 (input)
	sc_in <sc_logic>	pin2;	// OE1_ - G1_ (input)
	sc_in <sc_logic>	pin3;	// OE2_ - G2_ (input)
	sc_inout_resolved	pin4;	// I/O6 - DQ6 (tri_state)
	sc_inout_resolved	pin5;	// I/O4 - DQ4 (tri_state)
	sc_inout_resolved	pin6;	// I/O2 - DQ2 (tri_state)
	sc_inout_resolved	pin7;	// I/O0 - DQ0 (tri_state)
	sc_out <sc_logic>	pin8;	// Q0 - Q0 (output)
	sc_in <sc_logic>	pin9;	// MR_ - CLR_ (input)
	sc_in <sc_logic>	pin11;	// DS0 - RSI (input)
	sc_in <sc_logic>	pin12;	// CP - CLK (input)
	sc_inout_resolved	pin13;	// IO1 - DQ1 (tri_state)
	sc_inout_resolved	pin14;	// IO3 - DQ3 (tri_state)
	sc_inout_resolved	pin15;	// IO5 - DQ5 (tri_state)
	sc_inout_resolved	pin16;	// IO7 - DQ7 (tri_state)
	sc_out <sc_logic>	pin17;	// Q7 - Q7 (output)
	sc_in <sc_logic>	pin18;	// DS7 - LSI (input)
	sc_in <sc_logic>	pin19;	// S1 - S0 (input)

	SC_HAS_PROCESS(SCM_F299);

	SCM_F299(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin2 << pin3 << pin9 << pin12.pos();
		loadit(arg);
	}

	private:
	struct scm_f299_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_F299 */
