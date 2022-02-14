#ifndef R1000_F151
#define R1000_F151

// 8-Input Multiplexer
// Fairchild DS09481 April 1988 Revised September 2000


struct scm_f151_state;

SC_MODULE(SCM_F151)
{
	sc_in <sc_logic>	pin1;	// I3 - D (input)
	sc_in <sc_logic>	pin2;	// I2 - C (input)
	sc_in <sc_logic>	pin3;	// I1 - B (input)
	sc_in <sc_logic>	pin4;	// I0 - A (input)
	sc_out <sc_logic>	pin5;	// Z - Y (output)
	sc_out <sc_logic>	pin6;	// Z_ - Y_ (output)
	sc_in <sc_logic>	pin7;	// E_ - E_ (input)
	sc_in <sc_logic>	pin9;	// S2 - S0 (input)
	sc_in <sc_logic>	pin10;	// S1 - S1 (input)
	sc_in <sc_logic>	pin11;	// S0 - S2 (input)
	sc_in <sc_logic>	pin12;	// I7 - H (input)
	sc_in <sc_logic>	pin13;	// I6 - G (input)
	sc_in <sc_logic>	pin14;	// I5 - F (input)
	sc_in <sc_logic>	pin15;	// I4 - E (input)

	SC_HAS_PROCESS(SCM_F151);

	SCM_F151(sc_module_name nm, const char *arg);

	private:
	struct scm_f151_state *state;
	void doit(void);
};

#endif /* R1000_F151 */
