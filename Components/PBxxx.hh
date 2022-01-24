#ifndef R1000_PBXXX
#define R1000_PBXXX

// Signetics 82S123A, product specification, June 6 1985
// 32x8 PROM

struct scm_pbxxx_state;

SC_MODULE(SCM_PBXXX)
{
	sc_out <sc_logic>	pin1;	// Y7 (tri_state)
	sc_out <sc_logic>	pin2;	// Y6 (tri_state)
	sc_out <sc_logic>	pin3;	// Y5 (tri_state)
	sc_out <sc_logic>	pin4;	// Y4 (tri_state)
	sc_out <sc_logic>	pin5;	// Y3 (tri_state)
	sc_out <sc_logic>	pin6;	// Y2 (tri_state)
	sc_out <sc_logic>	pin7;	// Y1 (tri_state)
	sc_out <sc_logic>	pin9;	// Y0 (tri_state)
	sc_in <sc_logic>	pin10;	// A4 (input)
	sc_in <sc_logic>	pin11;	// A3 (input)
	sc_in <sc_logic>	pin12;	// A2 (input)
	sc_in <sc_logic>	pin13;	// A1 (input)
	sc_in <sc_logic>	pin14;	// A0 (input)
	sc_in <sc_logic>	pin15;	// OE_ (input)

	SC_HAS_PROCESS(SCM_PBXXX);

	SCM_PBXXX(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin10 << pin11 << pin12 << pin13 << pin14 << pin15;
		loadit(arg);
	}

	private:
	struct scm_pbxxx_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_PBXXX */
