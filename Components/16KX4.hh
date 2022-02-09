#ifndef R1000_16KX4
#define R1000_16KX4

// CY7C164 Cypress Document 38-05025 Rev. ** Revised August 24, 2001

struct scm_16kx4_state;

SC_MODULE(SCM_16KX4)
{
	sc_in <sc_logic>	pin1;	// A0 (input)
	sc_in <sc_logic>	pin2;	// A1 (input)
	sc_in <sc_logic>	pin3;	// A2 (input)
	sc_in <sc_logic>	pin4;	// A3 (input)
	sc_in <sc_logic>	pin5;	// A4 (input)
	sc_in <sc_logic>	pin6;	// A5 (input)
	sc_in <sc_logic>	pin7;	// A6 (input)
	sc_in <sc_logic>	pin8;	// A7 (input)
	sc_in <sc_logic>	pin9;	// A8 (input)
	sc_in <sc_logic>	pin10;	// CS_ (input)
	sc_in <sc_logic>	pin12;	// WE_ (input)
	sc_inout_resolved	pin13;	// IO0 (tri_state)
	sc_inout_resolved	pin14;	// IO1 (tri_state)
	sc_inout_resolved	pin15;	// IO2 (tri_state)
	sc_inout_resolved	pin16;	// IO3 (tri_state)
	sc_in <sc_logic>	pin17;	// A9 (input)
	sc_in <sc_logic>	pin18;	// A10 (input)
	sc_in <sc_logic>	pin19;	// A11 (input)
	sc_in <sc_logic>	pin20;	// A12 (input)
	sc_in <sc_logic>	pin21;	// A13 (input)

	SC_HAS_PROCESS(SCM_16KX4);

	SCM_16KX4(sc_module_name nm, const char *arg);

	private:
	struct scm_16kx4_state *state;
	void doit(void);
};

#endif /* R1000_16KX4 */
