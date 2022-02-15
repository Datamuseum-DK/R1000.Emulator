#ifndef R1000_F181
#define R1000_F181

// 4-Bit Arithmetic Logic Unit
// Fairchild DS009491 April 1988 Revised January 2004
// http://www.righto.com/2017/03/inside-vintage-74181-alu-chip-how-it.html

struct scm_f181_state;

SC_MODULE(SCM_F181)
{
	sc_in <sc_logic>	pin1;	// B0_ - B3 (input)
	sc_in <sc_logic>	pin2;	// A0_ - A3 (input)
	sc_in <sc_logic>	pin3;	// S3 - S0 (input)
	sc_in <sc_logic>	pin4;	// S2 - S1 (input)
	sc_in <sc_logic>	pin5;	// S1 - S2 (input)
	sc_in <sc_logic>	pin6;	// S0 - S3 (input)
	sc_in <sc_logic>	pin7;	// Cn - C1_ (input)
	sc_in <sc_logic>	pin8;	// M - M (input)
	sc_out <sc_logic>	pin9;	// F0_ - Y3 (output)
	sc_out <sc_logic>	pin10;	// F1_ - Y2 (output)
	sc_out <sc_logic>	pin11;	// F2_ - Y1 (output)
	sc_out <sc_logic>	pin13;	// F3_ - Y0 (output)
	sc_out <sc_logic>	pin14;	// A=B - A_eq_B (output)
	sc_out <sc_logic>	pin15;	// P_ - P (output)
	sc_out <sc_logic>	pin16;	// Cn+4 - C0_ (output)
	sc_out <sc_logic>	pin17;	// G_ - Q (output)
	sc_in <sc_logic>	pin18;	// B3_ - B0 (input)
	sc_in <sc_logic>	pin19;	// A3_ - A0 (input)
	sc_in <sc_logic>	pin20;	// B2_ - B1 (input)
	sc_in <sc_logic>	pin21;	// A2_ - A1 (input)
	sc_in <sc_logic>	pin22;	// B1_ - B2 (input)
	sc_in <sc_logic>	pin23;	// A1_ - A2 (input)

	SC_HAS_PROCESS(SCM_F181);

	SCM_F181(sc_module_name nm, const char *arg);

	private:
	struct scm_f181_state *state;
	void doit();
};

#endif /* R1000_F181 */
