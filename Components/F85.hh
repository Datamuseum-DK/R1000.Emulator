#ifndef R1000_F85
#define R1000_F85

// 4-bit magnitude comparator
// Philips IC15 1994 Sep 27


struct scm_f85_state;

SC_MODULE(SCM_F85)
{
	sc_in <sc_logic>	pin1;	// B3 - B0 (input)
	sc_in <sc_logic>	pin2;	// Ia<b - E_lt_ (input)
	sc_in <sc_logic>	pin3;	// Ia=b - E_eq_ (input)
	sc_in <sc_logic>	pin4;	// Ia>b - ECLK (input)
	sc_out <sc_logic>	pin5;	// A>B - ACLKB (output)
	sc_out <sc_logic>	pin6;	// A=B - A_eq_B (output)
	sc_out <sc_logic>	pin7;	// A<B - A_lt_B (output)
	sc_in <sc_logic>	pin9;	// B0 - B3 (input)
	sc_in <sc_logic>	pin10;	// A0 - A3 (input)
	sc_in <sc_logic>	pin11;	// B1 - B2 (input)
	sc_in <sc_logic>	pin12;	// A1 - A2 (input)
	sc_in <sc_logic>	pin13;	// A2 - A1 (input)
	sc_in <sc_logic>	pin14;	// B2 - B1 (input)
	sc_in <sc_logic>	pin15;	// A3 - A0 (input)

	SC_HAS_PROCESS(SCM_F85);

	SCM_F85(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3 << pin4 << pin9 << pin10
			  << pin11 << pin12 << pin13 << pin14 << pin15;
		loadit(arg);
	}

	private:
	struct scm_f85_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F85 */
