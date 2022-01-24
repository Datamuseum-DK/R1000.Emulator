#ifndef R1000_8641
#define R1000_8641

// DS8641 Quad Unified Bus Tranceiver
// National Semiconductor TL/F/5806  RRD-B30M36  January 1996

struct scm_8641_state;

SC_MODULE(SCM_8641)
{
	sc_inout_resolved	pin1;	// B2_ (tri_state)
	sc_in <sc_logic>	pin2;	// IN2 (input)
	sc_out <sc_logic>	pin3;	// OUT2 (output)
	sc_inout_resolved	pin4;	// B3_ (tri_state)
	sc_in <sc_logic>	pin5;	// IN3 (input)
	sc_out <sc_logic>	pin6;	// OUT3 (output)
	sc_in <sc_logic>	pin7;	// EN0_ (input)
	sc_in <sc_logic>	pin9;	// EN1_ (input)
	sc_out <sc_logic>	pin10;	// OUT1 (output)
	sc_in <sc_logic>	pin11;	// IN1 (input)
	sc_inout_resolved	pin12;	// B1_ (tri_state)
	sc_out <sc_logic>	pin13;	// OUT0 (output)
	sc_in <sc_logic>	pin14;	// IN0 (input)
	sc_inout_resolved	pin15;	// B0_ (tri_state)

	SC_HAS_PROCESS(SCM_8641);

	SCM_8641(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive
		    << pin7 << pin9
		    << pin15 << pin12 << pin1 << pin4
		    << pin14 << pin11 << pin2 << pin5;
		loadit(arg);
	}

	private:
	struct scm_8641_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_8641 */
