#ifndef R1000_F74
#define R1000_F74

// (Dual) D-Type Positive Edge-Triggered Flip-Flop
// Fairchild DS009469 April 1988 Revised September 2000


struct scm_f74_state;

SC_MODULE(SCM_F74)
{
	sc_in <sc_logic>	pin1;	// CL_ (input)
	sc_in <sc_logic>	pin2;	// D (input)
	sc_in <sc_logic>	pin3;	// CLK (input)
	sc_in <sc_logic>	pin4;	// PR_ (input)
	sc_out <sc_logic>	pin5;	// Q (output)
	sc_out <sc_logic>	pin6;	// Q_ (output)

	SC_HAS_PROCESS(SCM_F74);

	SCM_F74(sc_module_name nm, const char *arg);

	private:
	struct scm_f74_state *state;

	void doit(void);
};

#endif /* R1000_F74 */
