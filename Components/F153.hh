#ifndef R1000_F153
#define R1000_F153

// Dual 4-Input Multiplexer
// Fairchild DS009482 April 1988 Revised September 2000


struct scm_f153_state;

SC_MODULE(SCM_F153)
{
	sc_in <sc_logic>	pin1;	// Ea_ - E0_ (input)
	sc_in <sc_logic>	pin2;	// S1 - S0 (input)
	sc_in <sc_logic>	pin3;	// I3a - D0 (input)
	sc_in <sc_logic>	pin4;	// I2a - C0 (input)
	sc_in <sc_logic>	pin5;	// I1a - B0 (input)
	sc_in <sc_logic>	pin6;	// I0a - A0 (input)
	sc_out <sc_logic>	pin7;	// Za - Y0 (output)
	sc_out <sc_logic>	pin9;	// Zb - Y1 (output)
	sc_in <sc_logic>	pin10;	// I0b - A1 (input)
	sc_in <sc_logic>	pin11;	// I1b - B1 (input)
	sc_in <sc_logic>	pin12;	// I2b - C1 (input)
	sc_in <sc_logic>	pin13;	// I3b - D1 (input)
	sc_in <sc_logic>	pin14;	// S0 - S1 (input)
	sc_in <sc_logic>	pin15;	// Eb_ - E1_ (input)

	SC_HAS_PROCESS(SCM_F153);

	SCM_F153(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin6
			  << pin10 << pin11 << pin12 << pin13 << pin14 << pin15;
		loadit(arg);
	}

	private:
	struct scm_f153_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F153 */
