#ifndef R1000_F10
#define R1000_F10

// (Triple) 3-input NAND gate


struct scm_f10_state;

SC_MODULE(SCM_F10)
{
	sc_in <sc_logic>	pin1;	// D0 (input)
	sc_in <sc_logic>	pin2;	// D1 (input)
	sc_in <sc_logic>	pin3;	// D2 (input)
	sc_out <sc_logic>	pin4;	// Q (output)

	SC_HAS_PROCESS(SCM_F10);
	SCM_F10(sc_module_name nm, const char *arg);

	private:
	struct scm_f10_state *state;
	bool ostate;
	void doit(void);
};

#endif /* R1000_F10 */
