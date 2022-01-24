#ifndef R1000_F374
#define R1000_F374

// Octal D-Type Flip-Flop with 3-STATE Outputs
// Fairchild DS009524 May 1988 Revised September 2000


struct scm_f374_state;

SC_MODULE(SCM_F374)
{
	sc_in <sc_logic>	pin1;	// OE_ (input)
	sc_out <sc_logic>	pin2;	// Q0 (tri_state)
	sc_in <sc_logic>	pin3;	// D0 (input)
	sc_in <sc_logic>	pin4;	// D1 (input)
	sc_out <sc_logic>	pin5;	// Q1 (tri_state)
	sc_out <sc_logic>	pin6;	// Q2 (tri_state)
	sc_in <sc_logic>	pin7;	// D2 (input)
	sc_in <sc_logic>	pin8;	// D3 (input)
	sc_out <sc_logic>	pin9;	// Q3 (tri_state)
	sc_in <sc_logic>	pin11;	// CLK (input)
	sc_out <sc_logic>	pin12;	// Q4 (tri_state)
	sc_in <sc_logic>	pin13;	// D4 (input)
	sc_in <sc_logic>	pin14;	// D5 (input)
	sc_out <sc_logic>	pin15;	// Q5 (tri_state)
	sc_out <sc_logic>	pin16;	// Q6 (tri_state)
	sc_in <sc_logic>	pin17;	// D6 (input)
	sc_in <sc_logic>	pin18;	// D7 (input)
	sc_out <sc_logic>	pin19;	// Q7 (tri_state)

	SC_HAS_PROCESS(SCM_F374);

	SCM_F374(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_THREAD(doit);
		sensitive << pin1 << pin11.pos();
		loadit(arg);
	}

	private:
	struct scm_f374_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_F374 */
