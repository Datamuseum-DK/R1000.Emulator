#ifndef R1000_XPAR67
#define R1000_XPAR67

struct scm_xpar67_state;

#define XPAR67_PINS() \
	PIN(0, pin2) PIN(0, pin3) PIN(0, pin4) \
	\
	PIN(1, pin5) PIN(1, pin6) PIN(1, pin7) PIN(1, pin8) \
	PIN(1, pin9) PIN(1, pin10) PIN(1, pin11) PIN(1, pin12) \
	\
	PIN(2, pin13) PIN(2, pin14) PIN(2, pin15) PIN(2, pin16) \
	PIN(2, pin17) PIN(2, pin18) PIN(2, pin19) PIN(2, pin20) \
	\
	PIN(3, pin21) PIN(3, pin22) PIN(3, pin23) PIN(3, pin24) \
	PIN(3, pin25) PIN(3, pin26) PIN(3, pin27) PIN(3, pin28) \
	\
	PIN(4, pin29) PIN(4, pin30) PIN(4, pin31) PIN(4, pin32) \
	PIN(4, pin33) PIN(4, pin34) PIN(4, pin35) PIN(4, pin36) \
	\
	PIN(5, pin37) PIN(5, pin38) PIN(5, pin39) PIN(5, pin40) \
	PIN(5, pin41) PIN(5, pin42) PIN(5, pin43) PIN(5, pin44) \
	\
	PIN(6, pin45) PIN(6, pin46) PIN(6, pin47) PIN(6, pin48) \
	PIN(6, pin49) PIN(6, pin50) PIN(6, pin51) PIN(6, pin52) \
	PIN(6, pin53) PIN(6, pin54) PIN(6, pin55) \
	\
	PIN(7, pin56) PIN(7, pin57) PIN(7, pin58) PIN(7, pin59) \
	PIN(7, pin60) PIN(7, pin61) \
	\
	PIN(8, pin62) PIN(8, pin63) PIN(8, pin64) PIN(8, pin65) \
	PIN(8, pin66) PIN(8, pin67) PIN(8, pin68)

SC_MODULE(SCM_XPAR67)
{

	#define PIN(bit, pin_in) \
	sc_in <sc_logic>        pin_in;
	XPAR67_PINS()
	#undef PIN

	sc_out <sc_logic>       pin70;
	sc_out <sc_logic>       pin71;
	sc_out <sc_logic>       pin72;
	sc_out <sc_logic>       pin73;
	sc_out <sc_logic>       pin74;
	sc_out <sc_logic>       pin75;
	sc_out <sc_logic>       pin76;
	sc_out <sc_logic>       pin77;
	sc_out <sc_logic>       pin78;
	sc_out <sc_logic>       pin79;
	sc_out <sc_logic>       pin80;
	sc_out <sc_logic>       pin81;
	sc_out <sc_logic>       pin82;
	sc_out <sc_logic>       pin83;
	sc_out <sc_logic>       pin84;
	sc_out <sc_logic>       pin85;
	sc_out <sc_logic>       pin86;
	sc_out <sc_logic>       pin87;

	SC_HAS_PROCESS(SCM_XPAR67);

	SCM_XPAR67(sc_module_name nm, const char *arg);

	private:
	struct scm_xpar67_state *state;
	void doit(void);
};

#endif /* R1000_XPAR67 */
