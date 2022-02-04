#ifndef R1000_F258
#define R1000_F258

// Quad 2-Input Multiplexer with 3-STATE Outputs
// Fairchild DS009508 April 1988 Revised September 2000


struct scm_f258_state;

SC_MODULE(SCM_F258)
{
	sc_in <sc_logic>	pin1;	// S (input)
	sc_in <sc_logic>	pin2;	// A0 (input)
	sc_in <sc_logic>	pin3;	// B0 (input)
	sc_out_resolved		pin4;	// Y0_ (tri_state)
	sc_in <sc_logic>	pin5;	// A1 (input)
	sc_in <sc_logic>	pin6;	// B1 (input)
	sc_out_resolved		pin7;	// Y1_ (tri_state)
	sc_out_resolved		pin9;	// Y2_ (tri_state)
	sc_in <sc_logic>	pin10;	// B2 (input)
	sc_in <sc_logic>	pin11;	// A2 (input)
	sc_out_resolved		pin12;	// Y3_ (tri_state)
	sc_in <sc_logic>	pin13;	// B3 (input)
	sc_in <sc_logic>	pin14;	// A3 (input)
	sc_in <sc_logic>	pin15;	// OE_ (input)

	SC_HAS_PROCESS(SCM_F258);

	SCM_F258(sc_module_name nm, const char *arg);

	private:
	struct scm_f258_state *state;
	void doit(void);
};

#endif /* R1000_F258 */
