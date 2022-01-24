#ifndef R1000_F138
#define R1000_F138

// 1-of-8 decoder/demultiplexer
// Philips IC15 1991 Feb 14


struct scm_f138_state;

SC_MODULE(SCM_F138)
{
	sc_in <sc_logic>	pin1;	// A0 - S2 (input)
	sc_in <sc_logic>	pin2;	// A1 - S1 (input)
	sc_in <sc_logic>	pin3;	// A2 - S0 (input)
	sc_in <sc_logic>	pin4;	// E0_ - D1 (input)
	sc_in <sc_logic>	pin5;	// E1_ - D2 (input)
	sc_in <sc_logic>	pin6;	// E2 - D0 (input)
	sc_out <sc_logic>	pin7;	// Q7_ - Y7_ (tri_state)
	sc_out <sc_logic>	pin9;	// Q6_ - Y6_ (tri_state)
	sc_out <sc_logic>	pin10;	// Q5_ - Y5_ (tri_state)
	sc_out <sc_logic>	pin11;	// Q4_ - Y4_ (tri_state)
	sc_out <sc_logic>	pin12;	// Q3_ - Y3_ (tri_state)
	sc_out <sc_logic>	pin13;	// Q2_ - Y2_ (tri_state)
	sc_out <sc_logic>	pin14;	// Q1_ - Y1_ (tri_state)
	sc_out <sc_logic>	pin15;	// Q0_ - Y0_ (tri_state)

	SC_HAS_PROCESS(SCM_F138);

	SCM_F138(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin6;
		loadit(arg);
	}

	private:
	struct scm_f138_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F138 */
