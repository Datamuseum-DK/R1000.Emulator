#ifndef R1000_XSR12
#define R1000_XSR12

// 4-bit bidirectional universal shift register
// Philips 1989 Apr 04, IC15 Data Handbook


struct scm_xsr12_state;

SC_MODULE(SCM_XSR12)
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
	sc_out <sc_logic>	pin19;	// q0
	sc_out <sc_logic>	pin20;	// q1
	sc_out <sc_logic>	pin21;	// q2
	sc_out <sc_logic>	pin22;	// q3
	sc_out <sc_logic>	pin23;	// q4
	sc_out <sc_logic>	pin24;	// q5
	sc_out <sc_logic>	pin25;	// q6
	sc_out <sc_logic>	pin26;	// q7
	sc_out <sc_logic>	pin27;	// q8
	sc_out <sc_logic>	pin28;	// q9
	sc_out <sc_logic>	pin29;	// q10
	sc_out <sc_logic>	pin30;	// q11

	SC_HAS_PROCESS(SCM_XSR12);

	SCM_XSR12(sc_module_name nm, const char *arg);

	private:
	struct scm_xsr12_state *state;
	void doit(void);
};

#endif /* R1000_XSR12 */
