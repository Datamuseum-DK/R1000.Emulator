#ifndef R1000_58167
#define R1000_58167

struct scm_58167_state;

SC_MODULE(SCM_58167)
{
	sc_in <sc_logic>	pin1;	// CS_ (input)
	sc_in <sc_logic>	pin2;	// RD_ (input)
	sc_in <sc_logic>	pin3;	// WR_ (input)
	sc_in <sc_logic>	pin4;	// RDY_ (input)
	sc_in <sc_logic>	pin5;	// A4 (input)
	sc_in <sc_logic>	pin6;	// A3 (input)
	sc_in <sc_logic>	pin7;	// A2 (input)
	sc_in <sc_logic>	pin8;	// A1 (input)
	sc_in <sc_logic>	pin9;	// A0 (input)
	sc_in <sc_logic>	pin10;	// IN (input)
	sc_out <sc_logic>	pin11;	// OUT (output)
	sc_out <sc_logic>	pin13;	// INT (output)
	sc_in <sc_logic>	pin14;	// STBBY_ (input)
	sc_inout_resolved	pin15;	// D7 (tri_state)
	sc_inout_resolved	pin16;	// D6 (tri_state)
	sc_inout_resolved	pin17;	// D5 (tri_state)
	sc_inout_resolved	pin18;	// D4 (tri_state)
	sc_inout_resolved	pin19;	// D3 (tri_state)
	sc_inout_resolved	pin20;	// D2 (tri_state)
	sc_inout_resolved	pin21;	// D1 (tri_state)
	sc_inout_resolved	pin22;	// D0 (tri_state)
	sc_in <sc_logic>	pin23;	// DN_ (input)
	sc_in <sc_logic>	pin24;	// VDD (input)

	SC_HAS_PROCESS(SCM_58167);

	SCM_58167(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		cout << "MISSING SCM_58167 IMPLEMENTATION\n";
		SC_METHOD(doit);
		loadit(arg);
	}

	private:
	struct scm_58167_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_58167 */
