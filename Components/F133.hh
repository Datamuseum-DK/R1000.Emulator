#ifndef R1000_F133
#define R1000_F133

// 13-input NAND gate
// Philips IC15 1993 Jul 02


struct scm_f133_state;

SC_MODULE(SCM_F133)
{
	sc_in <sc_logic>	pin1;	// D6 (input)
	sc_in <sc_logic>	pin2;	// D7 (input)
	sc_in <sc_logic>	pin3;	// D8 (input)
	sc_in <sc_logic>	pin4;	// D9 (input)
	sc_in <sc_logic>	pin5;	// D10 (input)
	sc_in <sc_logic>	pin6;	// D11 (input)
	sc_in <sc_logic>	pin7;	// D12 (input)
	sc_out <sc_logic>	pin8;	// Q (output)
	sc_in <sc_logic>	pin10;	// D0 (input)
	sc_in <sc_logic>	pin11;	// D1 (input)
	sc_in <sc_logic>	pin12;	// D2 (input)
	sc_in <sc_logic>	pin13;	// D3 (input)
	sc_in <sc_logic>	pin14;	// D4 (input)
	sc_in <sc_logic>	pin15;	// D5 (input)

	SC_HAS_PROCESS(SCM_F133);

	SCM_F133(sc_module_name nm, const char *arg);

	private:
	struct scm_f133_state *state;
	void doit(void);
};

#endif /* R1000_F133 */
