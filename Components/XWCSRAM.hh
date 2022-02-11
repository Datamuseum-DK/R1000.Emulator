#ifndef R1000_XWCSRAM
#define R1000_XWCSRAM

struct scm_xwcsram_state;

SC_MODULE(SCM_XWCSRAM)
{
	sc_in <sc_logic>	pin1;	// D0 (input)
	sc_out <sc_logic>	pin2;	// Q0 (output)
	sc_in <sc_logic>	pin3;	// D1 (input)
	sc_out <sc_logic>	pin4;	// Q1 (output)
	sc_in <sc_logic>	pin5;	// D2 (input)
	sc_out <sc_logic>	pin6;	// Q2 (output)
	sc_in <sc_logic>	pin7;	// D3 (input)
	sc_out <sc_logic>	pin8;	// Q3 (output)
	sc_in <sc_logic>	pin9;	// D4 (input)
	sc_out <sc_logic>	pin10;	// Q4 (output)
	sc_in <sc_logic>	pin11;	// D5 (input)
	sc_out <sc_logic>	pin12;	// Q5 (output)
	sc_in <sc_logic>	pin13;	// D6 (input)
	sc_out <sc_logic>	pin14;	// Q6 (output)
	sc_in <sc_logic>	pin15;	// D7 (input)
	sc_out <sc_logic>	pin16;	// Q7 (output)
	sc_in <sc_logic>	pin17;	// A0 (input)
	sc_in <sc_logic>	pin18;	// A1 (input)
	sc_in <sc_logic>	pin19;	// A2 (input)
	sc_in <sc_logic>	pin20;	// A3 (input)
	sc_in <sc_logic>	pin21;	// A4 (input)
	sc_in <sc_logic>	pin22;	// A5 (input)
	sc_in <sc_logic>	pin23;	// A6 (input)
	sc_in <sc_logic>	pin24;	// A7 (input)
	sc_in <sc_logic>	pin25;	// A8 (input)
	sc_in <sc_logic>	pin26;	// A9 (input)
	sc_in <sc_logic>	pin27;	// A10 (input)
	sc_in <sc_logic>	pin28;	// A11 (input)
	sc_in <sc_logic>	pin29;	// A12 (input)
	sc_in <sc_logic>	pin30;	// A13 (input)
	sc_in <sc_logic>	pin31;	// WE_ (input)

	SC_HAS_PROCESS(SCM_XWCSRAM);

	SCM_XWCSRAM(sc_module_name nm, const char *arg);

	private:
	struct scm_xwcsram_state *state;
	void doit(void);
};

#endif /* R1000_XWCSRAM */
