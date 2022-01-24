#ifndef R1000_1489
#define R1000_1489

struct scm_1489_state;

SC_MODULE(SCM_1489)
{
	sc_in <sc_logic>	pin1;	// IN0 (input)
	sc_in <sc_logic>	pin2;	// RC0 (input)
	sc_out <sc_logic>	pin3;	// Y0_ (output)
	sc_in <sc_logic>	pin4;	// IN1 (input)
	sc_in <sc_logic>	pin5;	// RC1 (input)
	sc_out <sc_logic>	pin6;	// Y1_ (output)
	sc_out <sc_logic>	pin8;	// Y2_ (output)
	sc_in <sc_logic>	pin9;	// RC2 (input)
	sc_in <sc_logic>	pin10;	// IN2 (input)
	sc_out <sc_logic>	pin11;	// Y3_ (output)
	sc_in <sc_logic>	pin12;	// RC3 (input)
	sc_in <sc_logic>	pin13;	// IN3 (input)

	SC_HAS_PROCESS(SCM_1489);

	SCM_1489(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		cout << "MISSING SCM_1489 IMPLEMENTATION\n";
		SC_METHOD(doit);
		loadit(arg);
	}

	private:
	struct scm_1489_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_1489 */
