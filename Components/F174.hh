#ifndef R1000_F174
#define R1000_F174

// Hex D-Type Flip-Flop with Master Reset
// Fairchild DS009489 April 1988 Revised September 2000


struct scm_f174_state;

SC_MODULE(SCM_F174)
{
	sc_in <sc_logic>	pin1;	// CLR_ (input)
	sc_out <sc_logic>	pin2;	// Q0 (output)
	sc_in <sc_logic>	pin3;	// D0 (input)
	sc_in <sc_logic>	pin4;	// D1 (input)
	sc_out <sc_logic>	pin5;	// Q1 (output)
	sc_in <sc_logic>	pin6;	// D2 (input)
	sc_out <sc_logic>	pin7;	// Q2 (output)
	sc_in <sc_logic>	pin9;	// CLK (input)
	sc_out <sc_logic>	pin10;	// Q3 (output)
	sc_in <sc_logic>	pin11;	// D3 (input)
	sc_out <sc_logic>	pin12;	// Q4 (output)
	sc_in <sc_logic>	pin13;	// D4 (input)
	sc_in <sc_logic>	pin14;	// D5 (input)
	sc_out <sc_logic>	pin15;	// Q5 (output)

	SC_HAS_PROCESS(SCM_F174);

	SCM_F174(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_THREAD(doit);
		sensitive << pin1 << pin9.pos();
		loadit(arg);
	}

	private:
	struct scm_f174_state *state;
	void loadit(const char *arg);
	bool old_state[6];

	void doit(void);
};

#endif /* R1000_F174 */
