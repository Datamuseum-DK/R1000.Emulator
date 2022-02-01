#ifndef R1000_F04
#define R1000_F04

// (Hex) Inverter

struct scm_f04_state;

SC_MODULE(SCM_F04)
{
	sc_in <sc_logic>	pin1;	// D (input)
	sc_out <sc_logic>	pin2;	// Q (output)

	SC_HAS_PROCESS(SCM_F04);

	SCM_F04(sc_module_name nm, const char *arg);

	private:
	struct scm_f04_state *state;
	void doit(void);
};

#endif /* R1000_F04 */
