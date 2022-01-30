#ifndef R1000_F182
#define R1000_F182

// Carry Lookahead Generator
// Fairchild DS009492 April 1988 Revised June 2002

struct scm_f182_state;

SC_MODULE(SCM_F182)
{
	sc_in <sc_logic>	pin1;	// G1_ - G2 (input)
	sc_in <sc_logic>	pin2;	// P1_ - P2 (input)
	sc_in <sc_logic>	pin3;	// G0_ - G3 (input)
	sc_in <sc_logic>	pin4;	// P0_ - P3 (input)
	sc_in <sc_logic>	pin5;	// G3_ - G0 (input)
	sc_in <sc_logic>	pin6;	// P3_ - P0 (input)
	sc_out <sc_logic>	pin7;	// P_ - CP (output)
	sc_out <sc_logic>	pin9;	// CN+Z - CO1_ (output)
	sc_out <sc_logic>	pin10;	// G_ - CG (output)
	sc_out <sc_logic>	pin11;	// CN+Y - CO2_ (output)
	sc_out <sc_logic>	pin12;	// CN+X - CO3_ (output)
	sc_in <sc_logic>	pin13;	// CN - C1_ (input)
	sc_in <sc_logic>	pin14;	// G2_ - G1 (input)
	sc_in <sc_logic>	pin15;	// P2_ - P1 (input)

	SC_HAS_PROCESS(SCM_F182);

	SCM_F182(sc_module_name nm, const char *arg);

	private:
	struct scm_f182_state *state;
	void doit(void);
};

#endif /* R1000_F182 */
