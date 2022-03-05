#ifndef R1000_F652
#define R1000_F652

struct scm_F652_state;

SC_MODULE(SCM_F652)
{
	sc_in <sc_logic>	pin1;	// CAB (input)
	sc_in <sc_logic>	pin2;	// SAB (input)
	sc_in <sc_logic>	pin3;	// OEB_ (input)
	sc_inout_resolved	pin4;	// A0 (tri_state)
	sc_inout_resolved	pin5;	// A1 (tri_state)
	sc_inout_resolved	pin6;	// A2 (tri_state)
	sc_inout_resolved	pin7;	// A3 (tri_state)
	sc_inout_resolved	pin8;	// A4 (tri_state)
	sc_inout_resolved	pin9;	// A5 (tri_state)
	sc_inout_resolved	pin10;	// A6 (tri_state)
	sc_inout_resolved	pin11;	// A7 (tri_state)
	sc_inout_resolved	pin13;	// B7 (tri_state)
	sc_inout_resolved	pin14;	// B6 (tri_state)
	sc_inout_resolved	pin15;	// B5 (tri_state)
	sc_inout_resolved	pin16;	// B4 (tri_state)
	sc_inout_resolved	pin17;	// B3 (tri_state)
	sc_inout_resolved	pin18;	// B2 (tri_state)
	sc_inout_resolved	pin19;	// B1 (tri_state)
	sc_inout_resolved	pin20;	// B0 (tri_state)
	sc_in <sc_logic>	pin21;	// OEA_ (input)
	sc_in <sc_logic>	pin22;	// SBA (input)
	sc_in <sc_logic>	pin23;	// CBA (input)

	SC_HAS_PROCESS(SCM_F652);

	SCM_F652(sc_module_name nm, const char *arg);

	private:
	struct scm_F652_state *state;
	void doit(void);
};

#endif /* R1000_F652 */
