#ifndef R1000_F244
#define R1000_F244

// Octal buffers (3-state)
// Philips IC15 1994 Dec 05


struct scm_f244_state;

SC_MODULE(SCM_F244)
{
	sc_in <sc_logic>	pin1;	// OE0_ (input)
	sc_in <sc_logic>	pin2;	// I0 (input)
	sc_out <sc_logic>	pin3;	// Y7 (tri_state)
	sc_in <sc_logic>	pin4;	// I1 (input)
	sc_out <sc_logic>	pin5;	// Y6 (tri_state)
	sc_in <sc_logic>	pin6;	// I2 (input)
	sc_out <sc_logic>	pin7;	// Y5 (tri_state)
	sc_in <sc_logic>	pin8;	// I3 (input)
	sc_out <sc_logic>	pin9;	// Y4 (tri_state)
	sc_in <sc_logic>	pin11;	// I4 (input)
	sc_out <sc_logic>	pin12;	// Y3 (tri_state)
	sc_in <sc_logic>	pin13;	// I5 (input)
	sc_out <sc_logic>	pin14;	// Y2 (tri_state)
	sc_in <sc_logic>	pin15;	// I6 (input)
	sc_out <sc_logic>	pin16;	// Y1 (tri_state)
	sc_in <sc_logic>	pin17;	// I7 (input)
	sc_out <sc_logic>	pin18;	// Y0 (tri_state)
	sc_in <sc_logic>	pin19;	// OE1_ (input)

	SC_HAS_PROCESS(SCM_F244);

	SCM_F244(sc_module_name nm, const char *arg);

	private:
	struct scm_f244_state *state;

	void doit1(void);
	void doit2(void);
};

#endif /* R1000_F244 */
