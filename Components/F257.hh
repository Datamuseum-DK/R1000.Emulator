#ifndef R1000_F257
#define R1000_F257


struct scm_f257_state;

SC_MODULE(SCM_F257)
{
	sc_in <sc_logic>	pin1;	// S (input)
	sc_in <sc_logic>	pin2;	// A0 (input)
	sc_in <sc_logic>	pin3;	// B0 (input)
	sc_inout_resolved	pin4;	// Y0 (tri_state)
	sc_in <sc_logic>	pin5;	// A1 (input)
	sc_in <sc_logic>	pin6;	// B1 (input)
	sc_inout_resolved	pin7;	// Y1 (tri_state)
	sc_inout_resolved	pin9;	// Y2 (tri_state)
	sc_in <sc_logic>	pin10;	// B2 (input)
	sc_in <sc_logic>	pin11;	// A2 (input)
	sc_inout_resolved	pin12;	// Y3 (tri_state)
	sc_in <sc_logic>	pin13;	// B3 (input)
	sc_in <sc_logic>	pin14;	// A3 (input)
	sc_in <sc_logic>	pin15;	// OE_ (input)

	SC_HAS_PROCESS(SCM_F257);

	SCM_F257(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3 << pin5 << pin6
			  << pin10 << pin11 << pin13 << pin14 << pin15;
		loadit(arg);
	}

	private:
	struct scm_f257_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F257 */
