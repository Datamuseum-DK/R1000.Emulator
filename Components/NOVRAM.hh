#ifndef R1000_NOVRAM
#define R1000_NOVRAM


struct scm_novram_state;

SC_MODULE(SCM_NOVRAM)
{
	sc_in <sc_logic>	pin1;	// A7 (input)
	sc_in <sc_logic>	pin3;	// A3 (input)
	sc_in <sc_logic>	pin4;	// A2 (input)
	sc_in <sc_logic>	pin5;	// A1 (input)
	sc_in <sc_logic>	pin6;	// A0 (input)
	sc_in <sc_logic>	pin7;	// CS_ (input)
	sc_in <sc_logic>	pin8;	// A4 (input)
	sc_in <sc_logic>	pin9;	// STORE_ (input)
	sc_in <sc_logic>	pin10;	// RECALL_ (input)
	sc_in <sc_logic>	pin11;	// WE_ (input)
	sc_inout_resolved	pin12;	// DQ3 (tri_state)
	sc_inout_resolved	pin13;	// DQ2 (tri_state)
	sc_inout_resolved	pin14;	// DQ1 (tri_state)
	sc_inout_resolved	pin15;	// DQ0 (tri_state)
	sc_in <sc_logic>	pin16;	// A5 (input)
	sc_in <sc_logic>	pin17;	// A6 (input)

	SC_HAS_PROCESS(SCM_NOVRAM);

	SCM_NOVRAM(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin7 << pin9 << pin10 << pin11;
		loadit(this->name());	// To tell the boards apart
	}

	private:
	struct scm_novram_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_NOVRAM */
