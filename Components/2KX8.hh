#ifndef R1000_2KX8
#define R1000_2KX8

struct scm_2KX8_state;

SC_MODULE(SCM_2KX8)
{
	sc_in <sc_logic>	pin1;	// A7 (input)
	sc_in <sc_logic>	pin2;	// A6 (input)
	sc_in <sc_logic>	pin3;	// A5 (input)
	sc_in <sc_logic>	pin4;	// A4 (input)
	sc_in <sc_logic>	pin5;	// A3 (input)
	sc_in <sc_logic>	pin6;	// A2 (input)
	sc_in <sc_logic>	pin7;	// A1 (input)
	sc_in <sc_logic>	pin8;	// A0 (input)
	sc_inout_resolved	pin9;	// IO0 (tri_state)
	sc_inout_resolved	pin10;	// IO1 (tri_state)
	sc_inout_resolved	pin11;	// IO2 (tri_state)
	sc_inout_resolved	pin13;	// IO3 (tri_state)
	sc_inout_resolved	pin14;	// IO4 (tri_state)
	sc_inout_resolved	pin15;	// IO5 (tri_state)
	sc_inout_resolved	pin16;	// IO6 (tri_state)
	sc_inout_resolved	pin17;	// IO7 (tri_state)
	sc_in <sc_logic>	pin18;	// CS_ (input)
	sc_in <sc_logic>	pin19;	// A10 (input)
	sc_in <sc_logic>	pin20;	// OE_ (input)
	sc_in <sc_logic>	pin21;	// WE_ (input)
	sc_in <sc_logic>	pin22;	// A9 (input)
	sc_in <sc_logic>	pin23;	// A8 (input)

	SC_HAS_PROCESS(SCM_2KX8);

	SCM_2KX8(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin18 << pin20 << pin21
			<< pin9 << pin10 << pin11 << pin13 << pin14 << pin15 << pin16 << pin17
		;
		loadit(arg);
	}

	private:
	struct scm_2KX8_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_2KX8 */
