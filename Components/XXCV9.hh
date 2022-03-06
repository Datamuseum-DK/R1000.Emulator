#ifndef R1000_XXCV9
#define R1000_XXCV9

struct scm_xxcv9_state;

#define XXCV9_LPIN 8

#define XXCV9_PINS() \
	PIN(0, pin1, pin10) PIN(1, pin2, pin11) PIN(2, pin3, pin12) \
	PIN(3, pin4, pin13) PIN(4, pin5, pin14) PIN(5, pin6, pin15) \
	PIN(6, pin7, pin16) PIN(7, pin8, pin17) PIN(8, pin9, pin18)

#define XXCV9_OEA pin19
#define XXCV9_CBA pin20
#define XXCV9_SBA pin21
#define XXCV9_OEB pin22
#define XXCV9_CAB pin23
#define XXCV9_SAB pin24

SC_MODULE(SCM_XXCV9)
{

	#define PIN(bit, pin_a, pin_b) \
	sc_inout_resolved        pin_a;
	XXCV9_PINS()
	#undef PIN

	#define PIN(bit, pin_a, pin_b) \
	sc_inout_resolved       pin_b;
	XXCV9_PINS()
	#undef PIN

	sc_in <sc_logic>	XXCV9_OEA;
	sc_in <sc_logic>	XXCV9_CBA;
	sc_in <sc_logic>	XXCV9_SBA;
	sc_in <sc_logic>	XXCV9_OEB;
	sc_in <sc_logic>	XXCV9_CAB;
	sc_in <sc_logic>	XXCV9_SAB;

	SC_HAS_PROCESS(SCM_XXCV9);

	SCM_XXCV9(sc_module_name nm, const char *arg);

	private:
	struct scm_xxcv9_state *state;
	void doit(void);
};

#endif /* R1000_XXCV9 */
