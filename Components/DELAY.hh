#ifndef R1000_DELAY
#define R1000_DELAY

struct scm_delay_state;

SC_MODULE(SCM_DELAY)
{
	sc_in <sc_logic>	pin1;	// In (input)
	sc_out <sc_logic>	pin2;	// OUT (output)

	SC_HAS_PROCESS(SCM_DELAY);

	SCM_DELAY(sc_module_name nm, const char *arg);

	private:
	struct scm_delay_state *state;
	void doit(void);
};

#endif /* R1000_DELAY */
