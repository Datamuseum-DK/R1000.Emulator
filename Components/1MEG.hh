#ifndef R1000_1MEG
#define R1000_1MEG

struct scm_1MEG_state;

SC_MODULE(SCM_1MEG)
{
	sc_in <sc_logic>	pin1;	// D (input)
	sc_in <sc_logic>	pin2;	// WE_ (input)
	sc_in <sc_logic>	pin3;	// RAS_ (input)
	sc_in <sc_logic>	pin5;	// A10 (input)
	sc_in <sc_logic>	pin6;	// A9 (input)
	sc_in <sc_logic>	pin7;	// A8 (input)
	sc_in <sc_logic>	pin8;	// A7 (input)
	sc_in <sc_logic>	pin10;	// A6 (input)
	sc_in <sc_logic>	pin11;	// A5 (input)
	sc_in <sc_logic>	pin12;	// A4 (input)
	sc_in <sc_logic>	pin13;	// A3 (input)
	sc_in <sc_logic>	pin14;	// A2 (input)
	sc_in <sc_logic>	pin15;	// A1 (input)
	sc_in <sc_logic>	pin16;	// CAS_ (input)
	sc_inout_resolved	pin17;	// Q (tri_state)

	SC_HAS_PROCESS(SCM_1MEG);

	SCM_1MEG(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		cout << "MISSING SCM_1MEG IMPLEMENTATION\n";
		SC_METHOD(doit);
		loadit(arg);
	}

	private:
	struct scm_1MEG_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_1MEG */
