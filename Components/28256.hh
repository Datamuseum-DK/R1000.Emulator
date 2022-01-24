#ifndef R1000_28256
#define R1000_28256

struct scm_28256_state;

SC_MODULE(SCM_28256)
{
	sc_in <sc_logic>	pin1;	// VPP (input)
	sc_in <sc_logic>	pin2;	// A0 (input)
	sc_in <sc_logic>	pin3;	// A5 (input)
	sc_in <sc_logic>	pin4;	// A6 (input)
	sc_in <sc_logic>	pin5;	// A7 (input)
	sc_in <sc_logic>	pin6;	// A8 (input)
	sc_in <sc_logic>	pin7;	// A9 (input)
	sc_in <sc_logic>	pin8;	// A10 (input)
	sc_in <sc_logic>	pin9;	// A11 (input)
	sc_in <sc_logic>	pin10;	// A12 (input)
	sc_inout_resolved	pin11;	// Y7 (tri_state)
	sc_inout_resolved	pin12;	// Y6 (tri_state)
	sc_inout_resolved	pin13;	// Y5 (tri_state)
	sc_inout_resolved	pin15;	// Y4 (tri_state)
	sc_inout_resolved	pin16;	// Y3 (tri_state)
	sc_inout_resolved	pin17;	// Y2 (tri_state)
	sc_inout_resolved	pin18;	// Y1 (tri_state)
	sc_inout_resolved	pin19;	// Y0 (tri_state)
	sc_in <sc_logic>	pin20;	// CE_ (input)
	sc_in <sc_logic>	pin21;	// A2 (input)
	sc_in <sc_logic>	pin22;	// OE_ (input)
	sc_in <sc_logic>	pin23;	// A1 (input)
	sc_in <sc_logic>	pin24;	// A3 (input)
	sc_in <sc_logic>	pin25;	// A4 (input)
	sc_in <sc_logic>	pin26;	// NC (input)
	sc_in <sc_logic>	pin27;	// PGM_ (input)

	SC_HAS_PROCESS(SCM_28256);

	SCM_28256(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		cout << "MISSING SCM_28256 IMPLEMENTATION\n";
		SC_METHOD(doit);
		loadit(arg);
	}

	private:
	struct scm_28256_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_28256 */
