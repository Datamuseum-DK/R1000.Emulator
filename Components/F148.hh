#ifndef R1000_F148
#define R1000_F148

// 8-Line to 3-Line Priority Encoder
// Fairchild DS009480 April 1988 Revised September 2000


struct scm_f148_state;

SC_MODULE(SCM_F148)
{
	sc_in <sc_logic>	pin1;	// I4_ - I3_ (input)
	sc_in <sc_logic>	pin2;	// I5_ - I2_ (input)
	sc_in <sc_logic>	pin3;	// I6 - I1_ (input)
	sc_in <sc_logic>	pin4;	// I7 - I0_ (input)
	sc_in <sc_logic>	pin5;	// EI_ - E_ (input)
	sc_out <sc_logic>	pin6;	// A2_ - Y0 (output)
	sc_out <sc_logic>	pin7;	// A1_ - Y1 (output)
	sc_out <sc_logic>	pin9;	// A0_ - Y2 (output)
	sc_in <sc_logic>	pin10;	// I0_ - I7_ (input)
	sc_in <sc_logic>	pin11;	// I1_ - I6_ (input)
	sc_in <sc_logic>	pin12;	// I2_ - I5_ (input)
	sc_in <sc_logic>	pin13;	// I3_ - I4_ (input)
	sc_out <sc_logic>	pin14;	// GS_ - GS_ (output)
	sc_out <sc_logic>	pin15;	// EO_ - EZ_ (output)

	SC_HAS_PROCESS(SCM_F148);

	SCM_F148(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin10
			  << pin11 << pin12 << pin13;
		loadit(arg);
	}

	private:
	struct scm_f148_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F148 */
