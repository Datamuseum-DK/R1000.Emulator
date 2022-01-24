#ifndef R1000_F280
#define R1000_F280

// 9-Bit Parity Generator Checker
// Fairchild DS009512 April 1988 Revised September 2000


struct scm_f280_state;

SC_MODULE(SCM_F280)
{
	sc_in <sc_logic>	pin1;	// I6 (input)
	sc_in <sc_logic>	pin2;	// I7 (input)
	sc_in <sc_logic>	pin4;	// I8 (input)
	sc_out <sc_logic>	pin5;	// PEV (output)
	sc_out <sc_logic>	pin6;	// POD (output)
	sc_in <sc_logic>	pin8;	// I0 (input)
	sc_in <sc_logic>	pin9;	// I1 (input)
	sc_in <sc_logic>	pin10;	// I2 (input)
	sc_in <sc_logic>	pin11;	// I3 (input)
	sc_in <sc_logic>	pin12;	// I4 (input)
	sc_in <sc_logic>	pin13;	// I5 (input)

	SC_HAS_PROCESS(SCM_F280);

	SCM_F280(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin4 << pin8
			  << pin9 << pin10 << pin11 << pin12 << pin13;
		loadit(arg);
	}

	private:
	struct scm_f280_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_F280 */
