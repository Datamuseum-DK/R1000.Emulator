#ifndef R1000_F163
#define R1000_F163

// SYNCHRONOUS 4-BIT BINARY COUNTER
// Fairchild Rev 1.0.2 April 2007


struct scm_f163_state;

SC_MODULE(SCM_F163)
{
	sc_in <sc_logic>	pin1;	// SR_ - CLR_ (input)
	sc_in <sc_logic>	pin2;	// CP - CLK (input)
	sc_in <sc_logic>	pin3;	// P0 - D3 (input)
	sc_in <sc_logic>	pin4;	// P1 - D2 (input)
	sc_in <sc_logic>	pin5;	// P2 - D1 (input)
	sc_in <sc_logic>	pin6;	// P3 - D0 (input)
	sc_in <sc_logic>	pin7;	// CEP - ENP (input)
	sc_in <sc_logic>	pin9;	// PE_ - LD_ (input)
	sc_in <sc_logic>	pin10;	// CET - ENT (input)
	sc_out <sc_logic>	pin11;	// Q3 - Q0 (output)
	sc_out <sc_logic>	pin12;	// Q2 - Q1 (output)
	sc_out <sc_logic>	pin13;	// Q1 - Q2 (output)
	sc_out <sc_logic>	pin14;	// Q0 - Q3 (output)
	sc_out <sc_logic>	pin15;	// TC - C0 (output)

	SC_HAS_PROCESS(SCM_F163);

	SCM_F163(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin2.pos();
		loadit(arg);
	}

	private:
	struct scm_f163_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F163 */
