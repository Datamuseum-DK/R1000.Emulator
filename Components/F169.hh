#ifndef R1000_F169
#define R1000_F169

// 4-Stage Synchronous Bidirectional Counter
// Fairchild DS009488 April 1988 Revised September 2000


struct scm_f169_state;

SC_MODULE(SCM_F169)
{
	sc_in <sc_logic>	pin1;	// U/D_ - UP (input)
	sc_in <sc_logic>	pin2;	// CP - CLK (input)
	sc_in <sc_logic>	pin3;	// P0 - D3 (input)
	sc_in <sc_logic>	pin4;	// P1 - D2 (input)
	sc_in <sc_logic>	pin5;	// P2 - D1 (input)
	sc_in <sc_logic>	pin6;	// P3 - D0 (input)
	sc_in <sc_logic>	pin7;	// CEP_ - ENP_ (input)
	sc_in <sc_logic>	pin9;	// PE_ - LD_ (input)
	sc_in <sc_logic>	pin10;	// CET_ - ENT_ (input)
	sc_out <sc_logic>	pin11;	// Q3 - Q0 (output)
	sc_out <sc_logic>	pin12;	// Q2 - Q1 (output)
	sc_out <sc_logic>	pin13;	// Q1 - Q2 (output)
	sc_out <sc_logic>	pin14;	// Q0 - Q3 (output)
	sc_out <sc_logic>	pin15;	// TC_ - C0_ (output)

	SC_HAS_PROCESS(SCM_F169);

	SCM_F169(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin2.pos() << pin10 << pin1;
		loadit(arg);
	}

	private:
	struct scm_f169_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_F169 */
