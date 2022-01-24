#ifndef R1000_F64
#define R1000_F64

// 4-2-3-2-Input AND-OR-Invert Gate
// Fairchild DS009467 April 1988 Revised March 1999


struct scm_f64_state;

SC_MODULE(SCM_F64)
{
	sc_in <sc_logic>	pin1;	// D0 (input)
	sc_in <sc_logic>	pin2;	// A0 (input)
	sc_in <sc_logic>	pin3;	// A1 (input)
	sc_in <sc_logic>	pin4;	// C0 (input)
	sc_in <sc_logic>	pin5;	// C1 (input)
	sc_in <sc_logic>	pin6;	// C2 (input)
	sc_out <sc_logic>	pin8;	// Q (output)
	sc_in <sc_logic>	pin9;	// B0 (input)
	sc_in <sc_logic>	pin10;	// B1 (input)
	sc_in <sc_logic>	pin11;	// D1 (input)
	sc_in <sc_logic>	pin12;	// D2 (input)
	sc_in <sc_logic>	pin13;	// D3 (input)

	SC_HAS_PROCESS(SCM_F64);

	SCM_F64(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin6
			  << pin9 << pin10 << pin11 << pin12 << pin13;
		loadit(arg);
	}

	private:
	struct scm_f64_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F64 */
