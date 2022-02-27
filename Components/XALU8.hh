#ifndef R1000_XALU8
#define R1000_XALU8

// two times 4-Bit Arithmetic Logic Unit
// Fairchild DS009491 April 1988 Revised January 2004
// http://www.righto.com/2017/03/inside-vintage-74181-alu-chip-how-it.html

struct scm_xalu8_state;

SC_MODULE(SCM_XALU8)
{
	sc_in <sc_logic>	pin1;	// A0
	sc_in <sc_logic>	pin2;	// A1
	sc_in <sc_logic>	pin3;	// A2
	sc_in <sc_logic>	pin4;	// A3
	sc_in <sc_logic>	pin5;	// A4
	sc_in <sc_logic>	pin6;	// A5
	sc_in <sc_logic>	pin7;	// A6
	sc_in <sc_logic>	pin8;	// A7

	sc_in <sc_logic>	pin9;	// B0
	sc_in <sc_logic>	pin10;	// B1
	sc_in <sc_logic>	pin11;	// A2
	sc_in <sc_logic>	pin12;	// B3
	sc_in <sc_logic>	pin13;	// B4
	sc_in <sc_logic>	pin14;	// B5
	sc_in <sc_logic>	pin15;	// B6
	sc_in <sc_logic>	pin16;	// B7

	sc_out <sc_logic>	pin17;	// Y0
	sc_out <sc_logic>	pin18;	// Y1
	sc_out <sc_logic>	pin19;	// Y2
	sc_out <sc_logic>	pin20;	// Y3
	sc_out <sc_logic>	pin21;	// Y4
	sc_out <sc_logic>	pin22;	// Y5
	sc_out <sc_logic>	pin23;	// Y6
	sc_out <sc_logic>	pin24;	// Y7

	sc_in <sc_logic>	pin25;	// CI
	sc_out <sc_logic>	pin26;	// CO
	sc_out <sc_logic>	pin27;	// CO
	sc_in <sc_logic>	pin28;	// M
	sc_in <sc_logic>	pin29;	// S0
	sc_in <sc_logic>	pin30;	// S1
	sc_in <sc_logic>	pin31;	// S2
	sc_in <sc_logic>	pin32;	// S3

	SC_HAS_PROCESS(SCM_XALU8);

	SCM_XALU8(sc_module_name nm, const char *arg);

	private:
	struct scm_xalu8_state *state;
	void doit();
};

#endif /* R1000_XALU8 */
