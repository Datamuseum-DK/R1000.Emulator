#ifndef R1000_XDRAM9
#define R1000_XDRAM9

#define DQPINS9() \
	PIN(0, pin13) PIN(1, pin14) PIN(2, pin15) PIN(3, pin16) \
	PIN(4, pin17) PIN(5, pin18) PIN(6, pin19) PIN(7, pin20) \
	PIN(8, pin21)

struct scm_xdram9_state;

SC_MODULE(SCM_XDRAM9)
{
	sc_in <sc_logic>	pin1;	// WE_ (input)
	sc_in <sc_logic>	pin2;	// CAS (input)
	sc_in <sc_logic>	pin3;	// A0 (input)
	sc_in <sc_logic>	pin4;	// A1 (input)
	sc_in <sc_logic>	pin5;	// A2 (input)
	sc_in <sc_logic>	pin6;	// A3 (input)
	sc_in <sc_logic>	pin7;	// A4 (input)
	sc_in <sc_logic>	pin8;	// A5 (input)
	sc_in <sc_logic>	pin9;	// A6 (input)
	sc_in <sc_logic>	pin10;	// A7 (input)
	sc_in <sc_logic>	pin11;	// A8 (input)
	sc_in <sc_logic>	pin12;	// A9 (input)

	#define PIN(bit, pin_no) sc_inout_resolved pin_no;
	DQPINS9()
	#undef PIN
	sc_in <sc_logic>	pin22;	// RAS (input)

	SC_HAS_PROCESS(SCM_XDRAM9);

	SCM_XDRAM9(sc_module_name nm, const char *arg);

	private:
	struct scm_xdram9_state *state;
	void doit(void);
};

#endif /* R1000_XDRAM9 */
