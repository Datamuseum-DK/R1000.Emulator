#ifndef R1000_F240
#define R1000_F240

// Octal inverting buffers (3-state)
// Philips 2004 Feb 25


struct scm_f240_state;

SC_MODULE(SCM_F240)
{
	sc_in <sc_logic>	pin1;	// OE0_ (input)
	sc_in <sc_logic>	pin2;	// I0 (input)
	sc_out <sc_logic>	pin3;	// Y7_ (tri_state)
	sc_in <sc_logic>	pin4;	// I1 (input)
	sc_out <sc_logic>	pin5;	// Y6_ (tri_state)
	sc_in <sc_logic>	pin6;	// I2 (input)
	sc_out <sc_logic>	pin7;	// Y5_ (tri_state)
	sc_in <sc_logic>	pin8;	// I3 (input)
	sc_out <sc_logic>	pin9;	// Y4_ (tri_state)
	sc_in <sc_logic>	pin11;	// I4 (input)
	sc_out <sc_logic>	pin12;	// Y3_ (tri_state)
	sc_in <sc_logic>	pin13;	// I5 (input)
	sc_out <sc_logic>	pin14;	// Y2_ (tri_state)
	sc_in <sc_logic>	pin15;	// I6 (input)
	sc_out <sc_logic>	pin16;	// Y1_ (tri_state)
	sc_in <sc_logic>	pin17;	// I7 (input)
	sc_out <sc_logic>	pin18;	// Y0_ (tri_state)
	sc_in <sc_logic>	pin19;	// OE1_ (input)

	SC_HAS_PROCESS(SCM_F240);

	SCM_F240(sc_module_name nm, const char *arg, unsigned zdelay=0) : sc_module(nm)
	{
		SC_THREAD(doit1);
		sensitive << pin1 << pin2 << pin4 << pin6 << pin8;
		SC_THREAD(doit2);
		sensitive << pin19 << pin11 << pin13 << pin15 << pin17;
		loadit(arg, zdelay);
	}

	private:
	struct scm_f240_state *state;
	void loadit(const char *arg, unsigned zdelay);

	void doit1(void);
	void doit2(void);
};

#endif /* R1000_F240 */
