#ifndef R1000_25S558
#define R1000_25S558


struct scm_25s558_state;

SC_MODULE(SCM_25S558)
{
	sc_in <sc_logic>	pin1;	// A7 (input)
	sc_in <sc_logic>	pin2;	// A6 (input)
	sc_in <sc_logic>	pin3;	// A5 (input)
	sc_in <sc_logic>	pin4;	// A4 (input)
	sc_in <sc_logic>	pin5;	// A3 (input)
	sc_in <sc_logic>	pin6;	// A2 (input)
	sc_in <sc_logic>	pin7;	// A1 (input)
	sc_in <sc_logic>	pin8;	// A0 (input)
	sc_in <sc_logic>	pin9;	// RS (input)
	sc_in <sc_logic>	pin11;	// RU (input)
	sc_in <sc_logic>	pin12;	// B7 (input)
	sc_in <sc_logic>	pin13;	// B6 (input)
	sc_in <sc_logic>	pin14;	// B5 (input)
	sc_in <sc_logic>	pin15;	// B4 (input)
	sc_in <sc_logic>	pin16;	// B3 (input)
	sc_in <sc_logic>	pin17;	// B2 (input)
	sc_in <sc_logic>	pin18;	// B1 (input)
	sc_in <sc_logic>	pin19;	// B0 (input)
	sc_in <sc_logic>	pin20;	// BM (input)
	sc_in <sc_logic>	pin21;	// OE_ (input)
	sc_inout_resolved	pin22;	// Y0_ (tri_state)
	sc_inout_resolved	pin23;	// Y0 (tri_state)
	sc_inout_resolved	pin24;	// Y1 (tri_state)
	sc_inout_resolved	pin25;	// Y2 (tri_state)
	sc_inout_resolved	pin26;	// Y3 (tri_state)
	sc_inout_resolved	pin27;	// Y4 (tri_state)
	sc_inout_resolved	pin28;	// Y5 (tri_state)
	sc_inout_resolved	pin29;	// Y6 (tri_state)
	sc_inout_resolved	pin31;	// Y7 (tri_state)
	sc_inout_resolved	pin32;	// Y8 (tri_state)
	sc_inout_resolved	pin33;	// Y9 (tri_state)
	sc_inout_resolved	pin34;	// Y10 (tri_state)
	sc_inout_resolved	pin35;	// Y11 (tri_state)
	sc_inout_resolved	pin36;	// Y12 (tri_state)
	sc_inout_resolved	pin37;	// Y13 (tri_state)
	sc_inout_resolved	pin38;	// Y14 (tri_state)
	sc_inout_resolved	pin39;	// Y15 (tri_state)
	sc_in <sc_logic>	pin40;	// AM (input)

	SC_HAS_PROCESS(SCM_25S558);

	SCM_25S558(sc_module_name nm, const char *arg);

	private:
	struct scm_25s558_state *state;
	void doit(void);
};

#endif /* R1000_25S558 */
