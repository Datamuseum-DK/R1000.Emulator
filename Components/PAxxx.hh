#ifndef R1000_PAXXX
#define R1000_PAXXX


struct scm_paxxx_state;

SC_MODULE(SCM_PAXXX)
{
	sc_in <sc_logic>	pin1;	// A8 (input)
	sc_in <sc_logic>	pin2;	// A7 (input)
	sc_in <sc_logic>	pin3;	// A6 (input)
	sc_in <sc_logic>	pin4;	// A5 (input)
	sc_in <sc_logic>	pin5;	// A4 (input)
	sc_out <sc_logic>	pin6;	// Y7 (tri_state)
	sc_out <sc_logic>	pin7;	// Y6 (tri_state)
	sc_out <sc_logic>	pin8;	// Y5 (tri_state)
	sc_out <sc_logic>	pin9;	// Y4 (tri_state)
	sc_out <sc_logic>	pin11;	// Y3 (tri_state)
	sc_out <sc_logic>	pin12;	// Y2 (tri_state)
	sc_out <sc_logic>	pin13;	// Y1 (tri_state)
	sc_out <sc_logic>	pin14;	// Y0 (tri_state)
	sc_in <sc_logic>	pin15;	// OE_ (input)
	sc_in <sc_logic>	pin16;	// A3 (input)
	sc_in <sc_logic>	pin17;	// A2 (input)
	sc_in <sc_logic>	pin18;	// A1 (input)
	sc_in <sc_logic>	pin19;	// A0 (input)

	SC_HAS_PROCESS(SCM_PAXXX);

	SCM_PAXXX(sc_module_name nm, const char *arg);

	private:
	struct scm_paxxx_state *state;
	uint8_t prom[512];

	void doit(void);
};

#endif /* R1000_PAXXX */
