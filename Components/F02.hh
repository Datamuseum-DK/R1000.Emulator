#ifndef R1000_F02
#define R1000_F02

// (Quad) 2-input NOR


struct scm_f02_state;

SC_MODULE(SCM_F02)
{
	sc_in <sc_logic>	pin1;	// D0 (input)
	sc_in <sc_logic>	pin2;	// D1 (input)
	sc_out <sc_logic>	pin3;	// Q (output)

	SC_HAS_PROCESS(SCM_F02);

	SCM_F02(sc_module_name nm, const char *arg);
	private:
	struct scm_f02_state *state;

	void doit(void);
};

#endif /* R1000_F02 */
