#ifndef R1000_64KX1
#define R1000_64KX1

struct scm_64kx1_state;

SC_MODULE(SCM_64KX1)
{
	sc_in <sc_logic>	pin1;	// A0 (input)
	sc_in <sc_logic>	pin2;	// A1 (input)
	sc_in <sc_logic>	pin3;	// A2 (input)
	sc_in <sc_logic>	pin4;	// A3 (input)
	sc_in <sc_logic>	pin5;	// A4 (input)
	sc_in <sc_logic>	pin6;	// A5 (input)
	sc_in <sc_logic>	pin7;	// A6 (input)
	sc_in <sc_logic>	pin8;	// A7 (input)
	sc_out_resolved	pin9;		// Q (tri_state)
	sc_in <sc_logic>	pin10;	// WE_ (input)
	sc_in <sc_logic>	pin12;	// CS_ (input)
	sc_in <sc_logic>	pin13;	// D (input)
	sc_in <sc_logic>	pin14;	// A8 (input)
	sc_in <sc_logic>	pin15;	// A9 (input)
	sc_in <sc_logic>	pin16;	// A10 (input)
	sc_in <sc_logic>	pin17;	// A11 (input)
	sc_in <sc_logic>	pin18;	// A12 (input)
	sc_in <sc_logic>	pin19;	// A13 (input)
	sc_in <sc_logic>	pin20;	// A14 (input)
	sc_in <sc_logic>	pin21;	// A15 (input)

	SC_HAS_PROCESS(SCM_64KX1);

	SCM_64KX1(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive /* << pin10 */ << pin12 << pin13;
		loadit(arg);
	}

	private:
	struct scm_64kx1_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_64KX1 */
