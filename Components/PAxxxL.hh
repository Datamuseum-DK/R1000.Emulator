#ifndef R1000_PAXXXL
#define R1000_PAXXXL

struct scm_PAxxxL_state;

SC_MODULE(SCM_PAXXXL)
{
	sc_in <sc_logic>	pin1;	// A8 (input)
	sc_in <sc_logic>	pin2;	// A7 (input)
	sc_in <sc_logic>	pin3;	// A6 (input)
	sc_in <sc_logic>	pin4;	// A5 (input)
	sc_in <sc_logic>	pin5;	// A4 (input)
	sc_out <sc_logic>	pin6;	// Y7 (output)
	sc_out <sc_logic>	pin7;	// Y6 (output)
	sc_out <sc_logic>	pin8;	// Y5 (output)
	sc_out <sc_logic>	pin9;	// Y4 (output)
	sc_out <sc_logic>	pin11;	// Y3 (output)
	sc_out <sc_logic>	pin12;	// Y2 (output)
	sc_out <sc_logic>	pin13;	// Y1 (output)
	sc_out <sc_logic>	pin14;	// Y0 (output)
	sc_in <sc_logic>	pin15;	// CLK (input)
	sc_in <sc_logic>	pin16;	// A3 (input)
	sc_in <sc_logic>	pin17;	// A2 (input)
	sc_in <sc_logic>	pin18;	// A1 (input)
	sc_in <sc_logic>	pin19;	// A0 (input)

	SC_HAS_PROCESS(SCM_PAXXXL);

	SCM_PAXXXL(sc_module_name nm, const char *arg);

	private:
	struct scm_paxxxl_state *state;
	void doit(void);
	uint8_t prom[512];
};

#endif /* R1000_PAXXXL */
