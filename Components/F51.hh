#ifndef R1000_F51
#define R1000_F51

// (Dual) 2-wide 3-input AND-OR-invert gate
// Philips IC15 1989 Mar 03


struct scm_f51_state;

SC_MODULE(SCM_F51)
{
	sc_in <sc_logic>	pin1;	// A1 (input)
	sc_in <sc_logic>	pin2;	// A2 (input)
	sc_in <sc_logic>	pin3;	// A3 (input)
	sc_in <sc_logic>	pin4;	// B1 (input)
	sc_in <sc_logic>	pin5;	// B2 (input)
	sc_in <sc_logic>	pin6;	// B3 (input)
	sc_out <sc_logic>	pin7;	// Q (output)

	SC_HAS_PROCESS(SCM_F51);

	SCM_F51(sc_module_name nm, const char *arg);

	private:
	struct scm_f51_state *state;
	void doit(void);
};

#endif /* R1000_F51 */
