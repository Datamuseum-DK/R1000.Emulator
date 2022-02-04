#ifndef R1000_F20
#define R1000_F20

// (Dual) 4-input NAND gate
// Philips IC15 1989 Mar 03


struct scm_f20_state;

SC_MODULE(SCM_F20)
{
	sc_in <sc_logic>	pin1;	// D0 (input)
	sc_in <sc_logic>	pin2;	// D1 (input)
	sc_in <sc_logic>	pin4;	// D2 (input)
	sc_in <sc_logic>	pin5;	// D3 (input)
	sc_out <sc_logic>	pin6;	// Q (output)

	SC_HAS_PROCESS(SCM_F20);

	SCM_F20(sc_module_name nm, const char *arg);

	private:
	struct scm_f20_state *state;
	void doit(void);
};

#endif /* R1000_F20 */
