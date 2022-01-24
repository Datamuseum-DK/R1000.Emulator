#ifndef R1000_P2K8R
#define R1000_P2K8R


struct scm_p2k8r_state;

SC_MODULE(SCM_P2K8R)
{
	sc_in <sc_logic>	pin1;	// A3 (input)
	sc_in <sc_logic>	pin2;	// A4 (input)
	sc_in <sc_logic>	pin3;	// A5 (input)
	sc_in <sc_logic>	pin4;	// A6 (input)
	sc_in <sc_logic>	pin5;	// A7 (input)
	sc_in <sc_logic>	pin6;	// A8 (input)
	sc_in <sc_logic>	pin7;	// A9 (input)
	sc_in <sc_logic>	pin8;	// A10 (input)
	sc_inout_resolved	pin9;	// Y7 (tri_state)
	sc_inout_resolved	pin10;	// Y6 (tri_state)
	sc_inout_resolved	pin11;	// Y5 (tri_state)
	sc_inout_resolved	pin13;	// Y4 (tri_state)
	sc_inout_resolved	pin14;	// Y3 (tri_state)
	sc_inout_resolved	pin15;	// Y2 (tri_state)
	sc_inout_resolved	pin16;	// Y1 (tri_state)
	sc_inout_resolved	pin17;	// Y0 (tri_state)
	sc_in <sc_logic>	pin18;	// CK (input)
	sc_in <sc_logic>	pin19;	// OE_ (input)
	sc_in <sc_logic>	pin20;	// MR_ (input)
	sc_in <sc_logic>	pin21;	// A0 (input)
	sc_in <sc_logic>	pin22;	// A1 (input)
	sc_in <sc_logic>	pin23;	// A2 (input)

	SC_HAS_PROCESS(SCM_P2K8R);

	SCM_P2K8R(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin20 << pin19 << pin18.pos();
		loadit(arg);
	}

	private:
	struct scm_p2k8r_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_P2K8R */
