#ifndef R1000_XPAR64
#define R1000_XPAR64

struct scm_xpar64_state;

#define XPAR64_PINS() \
	PIN(0, pin2) PIN(0, pin3) PIN(0, pin4) PIN(0, pin5) \
	PIN(0, pin6) PIN(0, pin7) PIN(0, pin8) PIN(0, pin9) \
	PIN(1, pin10) PIN(1, pin11) PIN(1, pin12) PIN(1, pin13) \
	PIN(1, pin14) PIN(1, pin15) PIN(1, pin16) PIN(1, pin17) \
	PIN(2, pin18) PIN(2, pin19) PIN(2, pin20) PIN(2, pin21) \
	PIN(2, pin22) PIN(2, pin23) PIN(2, pin24) PIN(2, pin25) \
	PIN(3, pin26) PIN(3, pin27) PIN(3, pin28) PIN(3, pin29) \
	PIN(3, pin30) PIN(3, pin31) PIN(3, pin32) PIN(3, pin33) \
	PIN(4, pin34) PIN(4, pin35) PIN(4, pin36) PIN(4, pin37) \
	PIN(4, pin38) PIN(4, pin39) PIN(4, pin40) PIN(4, pin41) \
	PIN(5, pin42) PIN(5, pin43) PIN(5, pin44) PIN(5, pin45) \
	PIN(5, pin46) PIN(5, pin47) PIN(5, pin48) PIN(5, pin49) \
	PIN(6, pin50) PIN(6, pin51) PIN(6, pin52) PIN(6, pin53) \
	PIN(6, pin54) PIN(6, pin55) PIN(6, pin56) PIN(6, pin57) \
	PIN(7, pin58) PIN(7, pin59) PIN(7, pin60) PIN(7, pin61) \
	PIN(7, pin62) PIN(7, pin63) PIN(7, pin64) PIN(7, pin65)

SC_MODULE(SCM_XPAR64)
{

	sc_in <sc_logic>	pin1;	// ODD (input)

	#define PIN(bit, pin_in) \
	sc_in <sc_logic>        pin_in;
	XPAR64_PINS()
	#undef PIN

	sc_out <sc_logic>       pin66;
	sc_out <sc_logic>       pin67;
	sc_out <sc_logic>       pin68;
	sc_out <sc_logic>       pin69;
	sc_out <sc_logic>       pin70;
	sc_out <sc_logic>       pin71;
	sc_out <sc_logic>       pin72;
	sc_out <sc_logic>       pin73;

	SC_HAS_PROCESS(SCM_XPAR64);

	SCM_XPAR64(sc_module_name nm, const char *arg);

	private:
	struct scm_xpar64_state *state;
	void doit(void);
};

#endif /* R1000_XPAR64 */
