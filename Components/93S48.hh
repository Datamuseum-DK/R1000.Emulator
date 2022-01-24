#ifndef R1000_93S48
#define R1000_93S48


struct scm_93s48_state;

SC_MODULE(SCM_93S48)
{
	sc_in <sc_logic>	pin1;	// I5 (input)
	sc_in <sc_logic>	pin2;	// I6 (input)
	sc_in <sc_logic>	pin3;	// I7 (input)
	sc_in <sc_logic>	pin4;	// I8 (input)
	sc_in <sc_logic>	pin5;	// I9 (input)
	sc_in <sc_logic>	pin6;	// I10 (input)
	sc_in <sc_logic>	pin7;	// I11 (input)
	sc_out <sc_logic>	pin9;	// POD (output)
	sc_out <sc_logic>	pin10;	// PEV (output)
	sc_in <sc_logic>	pin11;	// I0 (input)
	sc_in <sc_logic>	pin12;	// I1 (input)
	sc_in <sc_logic>	pin13;	// I2 (input)
	sc_in <sc_logic>	pin14;	// I3 (input)
	sc_in <sc_logic>	pin15;	// I4 (input)

	SC_HAS_PROCESS(SCM_93S48);

	SCM_93S48(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin6
			  << pin7 << pin11 << pin12 << pin13 << pin14 << pin15;
		loadit(arg);
	}

	private:
	struct scm_93s48_state *state;
	void loadit(const char *arg);

	void doit(void);
};

#endif /* R1000_93S48 */
