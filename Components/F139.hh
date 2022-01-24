#ifndef R1000_F139
#define R1000_F139

// (Dual) 1-of-4 Decoder/Demultiplexer
// Fairchild DS009479 April 1988 Revised September 2000


struct scm_f139_state;

SC_MODULE(SCM_F139)
{
	sc_in <sc_logic>	pin1;	// E_ - E_ (input)
	sc_in <sc_logic>	pin2;	// A0a - B1 (input)
	sc_in <sc_logic>	pin3;	// A1a - B0 (input)
	sc_out <sc_logic>	pin4;	// Q0a_ - Y0_ (output)
	sc_out <sc_logic>	pin5;	// Q1a_ - Y1_ (output)
	sc_out <sc_logic>	pin6;	// Q2a_ - Y2_ (output)
	sc_out <sc_logic>	pin7;	// Q3a_ - Y3_ (output)

	SC_HAS_PROCESS(SCM_F139);

	SCM_F139(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3;
		loadit(arg);
	}

	private:
	struct scm_f139_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F139 */
