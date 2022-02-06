#ifndef R1000_F245AB
#define R1000_F245AB

// Octal Bidirectional Transceiver with 3-STATE Outputs
// Fairchild Rev 1.4.0 January 2008


struct scm_f245ab_state;

SC_MODULE(SCM_F245AB)
{
	sc_in <sc_logic>	pin1;	// DIR (input)
	sc_inout_resolved	pin2;	// B0 (tri_state)
	sc_inout_resolved	pin3;	// B1 (tri_state)
	sc_inout_resolved	pin4;	// B2 (tri_state)
	sc_inout_resolved	pin5;	// B3 (tri_state)
	sc_inout_resolved	pin6;	// B4 (tri_state)
	sc_inout_resolved	pin7;	// B5 (tri_state)
	sc_inout_resolved	pin8;	// B6 (tri_state)
	sc_inout_resolved	pin9;	// B7 (tri_state)
	sc_inout_resolved	pin11;	// A7 (tri_state)
	sc_inout_resolved	pin12;	// A6 (tri_state)
	sc_inout_resolved	pin13;	// A5 (tri_state)
	sc_inout_resolved	pin14;	// A4 (tri_state)
	sc_inout_resolved	pin15;	// A3 (tri_state)
	sc_inout_resolved	pin16;	// A2 (tri_state)
	sc_inout_resolved	pin17;	// A1 (tri_state)
	sc_inout_resolved	pin18;	// A0 (tri_state)
	sc_in <sc_logic>	pin19;	// OE_ (input)

	SC_HAS_PROCESS(SCM_F245AB);

	SCM_F245AB(sc_module_name nm, const char *arg);

	private:
	struct scm_f245ab_state *state;
	void doit(void);
};

#endif /* R1000_F245AB */
