#ifndef R1000_DLY_35
#define R1000_DLY_35

struct scm_DLY_35_state;

SC_MODULE(SCM_DLY_35)
{
	sc_in <sc_logic>	pin1;	// IN (input)
	sc_out <sc_logic>	pin4;	// 14NS (output)
	sc_out <sc_logic>	pin6;	// 28NS (output)
	sc_out <sc_logic>	pin8;	// 35NS (output)
	sc_out <sc_logic>	pin10;	// 21NS (output)
	sc_out <sc_logic>	pin12;	// 7NS (output)

	SC_HAS_PROCESS(SCM_DLY_35);

	SCM_DLY_35(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		cout << "MISSING SCM_DLY_35 IMPLEMENTATION\n";
		SC_METHOD(doit);
		loadit(arg);
	}

	private:
	struct scm_DLY_35_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_DLY_35 */
