#ifndef R1000_F113
#define R1000_F113

// (Dual) JK Negative Edge-Triggered Flip-Flop
// Fairchild DS009473 April 1988 Revised September 2000


struct scm_f113_state;

SC_MODULE(SCM_F113)
{
	sc_in <sc_logic>	pin1;	// CLK_ (input)
	sc_in <sc_logic>	pin2;	// K (input)
	sc_in <sc_logic>	pin3;	// J (input)
	sc_in <sc_logic>	pin4;	// PR_ (input)
	sc_out <sc_logic>	pin5;	// Q (output)
	sc_out <sc_logic>	pin6;	// Q_ (output)

	SC_HAS_PROCESS(SCM_F113);

	SCM_F113(sc_module_name nm, const char *arg);

	private:
	struct scm_f113_state *state;
	void doit(void);
};

#endif /* R1000_F113 */
