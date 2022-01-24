#ifndef R1000_2149
#define R1000_2149

struct scm_2149_state;

SC_MODULE(SCM_2149)
{
	sc_in <sc_logic>	pin1;	// A6 (input)
	sc_in <sc_logic>	pin2;	// A5 (input)
	sc_in <sc_logic>	pin3;	// A4 (input)
	sc_in <sc_logic>	pin4;	// A3 (input)
	sc_in <sc_logic>	pin5;	// A0 (input)
	sc_in <sc_logic>	pin6;	// A1 (input)
	sc_in <sc_logic>	pin7;	// A2 (input)
	sc_in <sc_logic>	pin8;	// CS_ (input)
	sc_in <sc_logic>	pin10;	// WE_ (input)
	sc_inout_resolved	pin11;	// DQ3 (tri_state)
	sc_inout_resolved	pin12;	// DQ2 (tri_state)
	sc_inout_resolved	pin13;	// DQ1 (tri_state)
	sc_inout_resolved	pin14;	// DQ0 (tri_state)
	sc_in <sc_logic>	pin15;	// A9 (input)
	sc_in <sc_logic>	pin16;	// A8 (input)
	sc_in <sc_logic>	pin17;	// A7 (input)

	SC_HAS_PROCESS(SCM_2149);

	SCM_2149(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin6 << pin7
		    << pin8 << pin10 << pin11 << pin12 << pin13 << pin14
		    << pin15 << pin16 << pin17;
		loadit(arg);
	}

	private:
	struct scm_2149_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_2149 */
