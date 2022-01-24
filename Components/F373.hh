#ifndef R1000_F373
#define R1000_F373

// Octal Transparent Latch with 3-STATE Outputs
// Fairchild DS009523 May 1988 Revised September 2000


struct scm_f373_state;

SC_MODULE(SCM_F373)
{
	sc_in <sc_logic>	pin1;	// OE_ (input)
	sc_out_resolved		pin2;	// Q0 (tri_state)
	sc_in <sc_logic>	pin3;	// D0 (input)
	sc_in <sc_logic>	pin4;	// D1 (input)
	sc_out_resolved		pin5;	// Q1 (tri_state)
	sc_out_resolved		pin6;	// Q2 (tri_state)
	sc_in <sc_logic>	pin7;	// D2 (input)
	sc_in <sc_logic>	pin8;	// D3 (input)
	sc_out_resolved		pin9;	// Q3 (tri_state)
	sc_in <sc_logic>	pin11;	// LE (input)
	sc_out_resolved		pin12;	// Q4 (tri_state)
	sc_in <sc_logic>	pin13;	// D4 (input)
	sc_in <sc_logic>	pin14;	// D5 (input)
	sc_out_resolved		pin15;	// Q5 (tri_state)
	sc_out_resolved		pin16;	// Q6 (tri_state)
	sc_in <sc_logic>	pin17;	// D6 (input)
	sc_in <sc_logic>	pin18;	// D7 (input)
	sc_out_resolved		pin19;	// Q7 (tri_state)

	SC_HAS_PROCESS(SCM_F373);

	SCM_F373(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin3 << pin4 << pin7 << pin8
			  << pin11 << pin13 << pin14 << pin17 << pin18;
		loadit(arg);
	}

	private:
	struct scm_f373_state *state;
	bool oreg[8];
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_F373 */
