#ifndef R1000_XSR8
#define R1000_XSR8

// 4-bit bidirectional universal shift register
// Philips 1989 Apr 04, IC15 Data Handbook


struct scm_xsr8_state;

SC_MODULE(SCM_XSR8)
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
	sc_out <sc_logic>	pin15;	// q0
	sc_out <sc_logic>	pin16;	// q1
	sc_out <sc_logic>	pin17;	// q2
	sc_out <sc_logic>	pin18;	// q3
	sc_out <sc_logic>	pin19;	// q4
	sc_out <sc_logic>	pin20;	// q5
	sc_out <sc_logic>	pin21;	// q6
	sc_out <sc_logic>	pin22;	// q7

	SC_HAS_PROCESS(SCM_XSR8);

	SCM_XSR8(sc_module_name nm, const char *arg);

	private:
	struct scm_xsr8_state *state;
	void doit(void);
};

#endif /* R1000_XSR8 */
