#ifndef R1000_F251
#define R1000_F251

// 8-Input Multiplexer with 3-STATE Outputs
// Fairchild DS009504 April 1988 Revised September 2000


struct scm_f251_state;

SC_MODULE(SCM_F251)
{
	sc_in <sc_logic>	pin1;	// I3 - D (input)
	sc_in <sc_logic>	pin2;	// I2 - C (input)
	sc_in <sc_logic>	pin3;	// I1 - B (input)
	sc_in <sc_logic>	pin4;	// I0 - A (input)
	sc_out_resolved		pin5;	// Z - Y (tri_state)
	sc_out_resolved		pin6;	// Z_ - Y_ (tri_state)
	sc_in <sc_logic>	pin7;	// OE_ - OE_ (input)
	sc_in <sc_logic>	pin9;	// S2 - S0 (input)
	sc_in <sc_logic>	pin10;	// S1 - S1 (input)
	sc_in <sc_logic>	pin11;	// S0 - S2 (input)
	sc_in <sc_logic>	pin12;	// I7 - H (input)
	sc_in <sc_logic>	pin13;	// I6 - G (input)
	sc_in <sc_logic>	pin14;	// I5 - F (input)
	sc_in <sc_logic>	pin15;	// I4 - E (input)

	SC_HAS_PROCESS(SCM_F251);

	SCM_F251(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3 << pin4 << pin7 << pin9
			  << pin10 << pin11 << pin12 << pin13 << pin14 << pin15;
		loadit(arg);
	}

	private:
	struct scm_f251_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F251 */
