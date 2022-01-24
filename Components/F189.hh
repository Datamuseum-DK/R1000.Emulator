#ifndef R1000_F189
#define R1000_F189

// 64-Bit Random Access Memory with 3-STATE Outputs
// Fairchild DS009493 April 1988 Revised January 2004


struct scm_f189_state;

SC_MODULE(SCM_F189)
{
	sc_in <sc_logic>	pin1;	// A0 (input)
	sc_in <sc_logic>	pin2;	// CS_ (input)
	sc_in <sc_logic>	pin3;	// WE_ (input)
	sc_in <sc_logic>	pin4;	// D0 (input)
	sc_out_resolved		pin5;	// Q0_ (tri_state)
	sc_in <sc_logic>	pin6;	// D1 (input)
	sc_out_resolved		pin7;	// Q1_ (tri_state)
	sc_out_resolved		pin9;	// Q2_ (tri_state)
	sc_out_resolved		pin11;	// Q3_ (tri_state)
	sc_in <sc_logic>	pin12;	// D3 (input)
	sc_in <sc_logic>	pin13;	// A3 (input)
	sc_in <sc_logic>	pin14;	// A2 (input)
	sc_in <sc_logic>	pin15;	// A1 (input)
	sc_in <sc_logic>	pin18;	// D2 (input)

	SC_HAS_PROCESS(SCM_F189);

	SCM_F189(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3 << pin4 << pin6 << pin12
			  << pin13 << pin14 << pin15 << pin18;
		loadit(arg);
	}

	private:
	struct scm_f189_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_F189 */
