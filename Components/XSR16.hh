#ifndef R1000_XSR16
#define R1000_XSR16

// (quad) 4-bit bidirectional universal shift register
// Philips 1989 Apr 04, IC15 Data Handbook

struct scm_xsr16_state;

SC_MODULE(SCM_XSR16)
{
	sc_in <sc_logic>	pin1;	// clk
	sc_in <sc_logic>	pin2;	// clr
	sc_in <sc_logic>	pin3;	// rsi
	sc_in <sc_logic>	pin4;	// lsi
	sc_in <sc_logic>	pin5;	// s0
	sc_in <sc_logic>	pin6;	// s1
	sc_in <sc_logic>	pin7;	// d0
	sc_in <sc_logic>	pin8;	// d1
	sc_in <sc_logic>	pin9;	// d2
	sc_in <sc_logic>	pin10;	// d3
	sc_in <sc_logic>	pin11;	// d4
	sc_in <sc_logic>	pin12;	// d5
	sc_in <sc_logic>	pin13;	// d6
	sc_in <sc_logic>	pin14;	// d7
	sc_in <sc_logic>	pin15;	// d8
	sc_in <sc_logic>	pin16;	// d9
	sc_in <sc_logic>	pin17;	// d10
	sc_in <sc_logic>	pin18;	// d11
	sc_in <sc_logic>	pin19;	// d12
	sc_in <sc_logic>	pin20;	// d13
	sc_in <sc_logic>	pin21;	// d14
	sc_in <sc_logic>	pin22;	// d15

	sc_out <sc_logic>	pin23;	// q0
	sc_out <sc_logic>	pin24;	// q1
	sc_out <sc_logic>	pin25;	// q2
	sc_out <sc_logic>	pin26;	// q3
	sc_out <sc_logic>	pin27;	// q4
	sc_out <sc_logic>	pin28;	// q5
	sc_out <sc_logic>	pin29;	// q6
	sc_out <sc_logic>	pin30;	// q7
	sc_out <sc_logic>	pin31;	// q8
	sc_out <sc_logic>	pin32;	// q9
	sc_out <sc_logic>	pin33;	// q10
	sc_out <sc_logic>	pin34;	// q11
	sc_out <sc_logic>	pin35;	// q12
	sc_out <sc_logic>	pin36;	// q13
	sc_out <sc_logic>	pin37;	// q14
	sc_out <sc_logic>	pin38;	// q15

	SC_HAS_PROCESS(SCM_XSR16);

	SCM_XSR16(sc_module_name nm, const char *arg);

	private:
	struct scm_xsr16_state *state;
	void doit(void);
};

#endif /* R1000_XSR16 */
