#ifndef R1000_F175
#define R1000_F175

// Quad D-Type Flip-Flop
// Fairchild DS009490 April 1988 Revised September 2000


struct scm_f175_state;

SC_MODULE(SCM_F175)
{
	sc_in <sc_logic>	pin1;	// CLR_ (input)
	sc_out <sc_logic>	pin2;	// Q0 (output)
	sc_out <sc_logic>	pin3;	// Q0_ (output)
	sc_in <sc_logic>	pin4;	// D0 (input)
	sc_in <sc_logic>	pin5;	// D1 (input)
	sc_out <sc_logic>	pin6;	// Q1_ (output)
	sc_out <sc_logic>	pin7;	// Q1 (output)
	sc_in <sc_logic>	pin9;	// CLK (input)
	sc_out <sc_logic>	pin10;	// Q2 (output)
	sc_out <sc_logic>	pin11;	// Q2_ (output)
	sc_in <sc_logic>	pin12;	// D2 (input)
	sc_in <sc_logic>	pin13;	// D3 (input)
	sc_out <sc_logic>	pin14;	// Q3_ (output)
	sc_out <sc_logic>	pin15;	// Q3 (output)

	SC_HAS_PROCESS(SCM_F175);

	SCM_F175(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_THREAD(doit);
		sensitive << pin1 << pin9.pos();
		loadit(arg);
	}

	private:
	struct scm_f175_state *state;
	void loadit(const char *arg);
	bool old_state[4];

	void doit(void);
};

#endif /* R1000_F175 */
