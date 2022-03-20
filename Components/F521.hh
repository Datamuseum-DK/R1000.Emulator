#ifndef R1000_F521
#define R1000_F521

// 8-Bit Identity Comparator
// Fairchild DS009545 April 1988 Revised October 2000


struct scm_f521_state;

SC_MODULE(SCM_F521)
{
	sc_in <sc_logic>	pin1;	// E_ (input)
	sc_in <sc_logic>	pin2;	// A0 (input)
	sc_in <sc_logic>	pin3;	// B0 (input)
	sc_in <sc_logic>	pin4;	// A1 (input)
	sc_in <sc_logic>	pin5;	// B1 (input)
	sc_in <sc_logic>	pin6;	// A2 (input)
	sc_in <sc_logic>	pin7;	// B2 (input)
	sc_in <sc_logic>	pin8;	// A3 (input)
	sc_in <sc_logic>	pin9;	// B3 (input)
	sc_in <sc_logic>	pin11;	// A4 (input)
	sc_in <sc_logic>	pin12;	// B4 (input)
	sc_in <sc_logic>	pin13;	// A5 (input)
	sc_in <sc_logic>	pin14;	// B5 (input)
	sc_in <sc_logic>	pin15;	// A6 (input)
	sc_in <sc_logic>	pin16;	// B6 (input)
	sc_in <sc_logic>	pin17;	// A7 (input)
	sc_in <sc_logic>	pin18;	// B7 (input)
	sc_out <sc_logic>	pin19;	// A_eq_B_ (output)

	SC_HAS_PROCESS(SCM_F521);

	SCM_F521(sc_module_name nm, const char *arg);

	private:
	struct scm_f521_state *state;
	void doit(void);
};

#endif /* R1000_F521 */
