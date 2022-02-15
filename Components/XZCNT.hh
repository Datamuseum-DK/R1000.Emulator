#ifndef R1000_XZCNT
#define R1000_XZCNT

struct scm_xzcnt_state;

#define XZCNT_I_PINS() \
	PIN(0, pin3) PIN(1, pin4) PIN(2, pin5) \
	PIN(3, pin6) PIN(4, pin7) PIN(5, pin8) \
	PIN(6, pin9) PIN(7, pin10) PIN(8, pin11) \
	PIN(9, pin12) PIN(10, pin13) PIN(11, pin14) \
	PIN(12, pin15) PIN(13, pin16) PIN(14, pin17) \
	PIN(15, pin18) PIN(16, pin19) PIN(17, pin20) \
	PIN(18, pin21) PIN(19, pin22) PIN(20, pin23) \
	PIN(21, pin24) PIN(22, pin25) PIN(23, pin26) \
	PIN(24, pin27) PIN(25, pin28) PIN(26, pin29) \
	PIN(27, pin30) PIN(28, pin31) PIN(29, pin32) \
	PIN(30, pin33) PIN(31, pin34) PIN(32, pin35) \
	PIN(33, pin36) PIN(34, pin37) PIN(35, pin38) \
	PIN(36, pin39) PIN(37, pin40) PIN(38, pin41) \
	PIN(39, pin42) PIN(40, pin43) PIN(41, pin44) \
	PIN(42, pin45) PIN(43, pin46) PIN(44, pin47) \
	PIN(45, pin48) PIN(46, pin49) PIN(47, pin50) \
	PIN(48, pin51) PIN(49, pin52) PIN(50, pin53) \
	PIN(51, pin54) PIN(52, pin55) PIN(53, pin56) \
	PIN(54, pin57) PIN(55, pin58) PIN(56, pin59) \
	PIN(57, pin60) PIN(58, pin61) PIN(59, pin62) \
	PIN(60, pin63) PIN(61, pin64) PIN(62, pin65) \
	PIN(63, pin66)

#define XZCNT_O_PINS() \
	PIN(0, pin67) \
	PIN(1, pin68) \
	PIN(2, pin69) \
	PIN(3, pin70) \
	PIN(4, pin71) \
	PIN(5, pin72) \
	PIN(6, pin73) \
	PIN(7, pin74) \
	PIN(8, pin75) \
	PIN(9, pin76) \
	PIN(10, pin77) \
	PIN(11, pin78) \
	PIN(12, pin79) \
	PIN(13, pin80) \
	PIN(14, pin81) \
	PIN(15, pin82)

SC_MODULE(SCM_XZCNT)
{
	sc_in <sc_logic>	pin1;	// CLK_ (input)
	sc_in <sc_logic>	pin2;	// OE_ (input)

	#define PIN(bit, pin_in) \
	sc_in <sc_logic>	pin_in;
	XZCNT_I_PINS()
	#undef PIN

	#define PIN(bit, pin_out) \
	sc_out <sc_logic>	pin_out;
	XZCNT_O_PINS()
	#undef PIN

	SC_HAS_PROCESS(SCM_XZCNT);

	SCM_XZCNT(sc_module_name nm, const char *arg);

	private:
	struct scm_xzcnt_state *state;
	void doit(void);
};

#endif /* R1000_XZCNT */
