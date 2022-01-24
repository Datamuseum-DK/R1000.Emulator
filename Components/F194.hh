#ifndef R1000_F194
#define R1000_F194

// 4-bit bidirectional universal shift register
// Philips 1989 Apr 04, IC15 Data Handbook


struct scm_f194_state;

SC_MODULE(SCM_F194)
{
	sc_in <sc_logic>	pin1;	// MR_ - CLR_ (input)
	sc_in <sc_logic>	pin2;	// DSR - RSI (input)
	sc_in <sc_logic>	pin3;	// D0 - D0 (input)
	sc_in <sc_logic>	pin4;	// D1 - D1 (input)
	sc_in <sc_logic>	pin5;	// D2 - D2 (input)
	sc_in <sc_logic>	pin6;	// D3 - D3 (input)
	sc_in <sc_logic>	pin7;	// DSL - LSI (input)
	sc_in <sc_logic>	pin9;	// S0 - S0 (input)
	sc_in <sc_logic>	pin10;	// S1 - S1 (input)
	sc_in <sc_logic>	pin11;	// CP - CLK (input)
	sc_out <sc_logic>	pin12;	// Q3 - Q3 (output)
	sc_out <sc_logic>	pin13;	// Q2 - Q2 (output)
	sc_out <sc_logic>	pin14;	// Q1 - Q1 (output)
	sc_out <sc_logic>	pin15;	// Q0 - Q0 (output)

	SC_HAS_PROCESS(SCM_F194);

	SCM_F194(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		(void)arg;
		SC_THREAD(doit);
		sensitive << pin1 << pin11.pos();
		loadit(arg);
	}

	private:
	struct scm_f194_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_F194 */
