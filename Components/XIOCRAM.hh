#ifndef R1000_XIOCRAM
#define R1000_XIOCRAM

struct scm_xiocram_state;

SC_MODULE(SCM_XIOCRAM)
{
	sc_in <sc_logic>	pin1;	// A0 (input)
	sc_in <sc_logic>	pin2;	// A1 (input)
	sc_in <sc_logic>	pin3;	// A2 (input)
	sc_in <sc_logic>	pin4;	// A3 (input)
	sc_in <sc_logic>	pin5;	// A4 (input)
	sc_in <sc_logic>	pin6;	// A5 (input)
	sc_in <sc_logic>	pin7;	// A6 (input)
	sc_in <sc_logic>	pin8;	// A7 (input)
	sc_in <sc_logic>	pin9;	// A8 (input)
	sc_in <sc_logic>	pin10;	// A9 (input)
	sc_in <sc_logic>	pin11;	// A10 (input)
	sc_in <sc_logic>	pin12;	// A11 (input)
	sc_in <sc_logic>	pin13;	// A12 (input)
	sc_in <sc_logic>	pin14;	// A13 (input)
	sc_in <sc_logic>	pin15;	// A14 (input)
	sc_in <sc_logic>	pin16;	// A15 (input)
	sc_in <sc_logic>	pin17;	// CS0_ (input)
	sc_in <sc_logic>	pin18;	// CS1_ (input)
	sc_in <sc_logic>	pin19;	// WE0_ (input)
	sc_in <sc_logic>	pin20;	// WE1_ (input)
	sc_in <sc_logic>	pin21;	// WE2_ (input)
	sc_in <sc_logic>	pin22;	// WE3_ (input)
	sc_in <sc_logic>	pin23;	// D00 (input)
	sc_in <sc_logic>	pin24;	// D01 (input)
	sc_in <sc_logic>	pin25;	// D02 (input)
	sc_in <sc_logic>	pin26;	// D03 (input)
	sc_in <sc_logic>	pin27;	// D04 (input)
	sc_in <sc_logic>	pin28;	// D05 (input)
	sc_in <sc_logic>	pin29;	// D06 (input)
	sc_in <sc_logic>	pin30;	// D07 (input)
	sc_in <sc_logic>	pin31;	// D20 (input)
	sc_in <sc_logic>	pin32;	// D21 (input)
	sc_in <sc_logic>	pin33;	// D22 (input)
	sc_in <sc_logic>	pin34;	// D23 (input)
	sc_in <sc_logic>	pin35;	// D18 (input)
	sc_in <sc_logic>	pin36;	// D25 (input)
	sc_in <sc_logic>	pin37;	// D26 (input)
	sc_in <sc_logic>	pin38;	// D27 (input)
	sc_in <sc_logic>	pin39;	// D34 (input)
	sc_in <sc_logic>	pin40;	// D35 (input)
	sc_in <sc_logic>	pin41;	// D36 (input)
	sc_in <sc_logic>	pin42;	// D37 (input)
	sc_in <sc_logic>	pin43;	// D38 (input)
	sc_out <sc_logic>	pin59;	// Q00 (output)
	sc_out <sc_logic>	pin60;	// Q01 (output)
	sc_out <sc_logic>	pin61;	// Q02 (output)
	sc_out <sc_logic>	pin62;	// Q03 (output)
	sc_out <sc_logic>	pin63;	// Q04 (output)
	sc_out <sc_logic>	pin64;	// Q05 (output)
	sc_out <sc_logic>	pin65;	// Q06 (output)
	sc_out <sc_logic>	pin66;	// Q07 (output)
	sc_out <sc_logic>	pin67;	// Q08 (output)
	sc_out <sc_logic>	pin68;	// Q15 (output)
	sc_out <sc_logic>	pin69;	// Q22 (output)
	sc_out <sc_logic>	pin70;	// Q17 (output)
	sc_out <sc_logic>	pin71;	// Q18 (output)
	sc_out <sc_logic>	pin72;	// Q25 (output)
	sc_out <sc_logic>	pin73;	// Q26 (output)
	sc_out <sc_logic>	pin74;	// Q27 (output)
	sc_out <sc_logic>	pin75;	// Q28 (output)
	sc_out <sc_logic>	pin76;	// Q35 (output)
	sc_out <sc_logic>	pin77;	// Q36 (output)
	sc_out <sc_logic>	pin78;	// Q37 (output)
	sc_out <sc_logic>	pin79;	// Q38 (output)

	SC_HAS_PROCESS(SCM_XIOCRAM);

	SCM_XIOCRAM(sc_module_name nm, const char *arg);

	private:
	struct scm_xiocram_state *state;
	void doit(void);
};

#endif /* R1000_XIOCRAM */
