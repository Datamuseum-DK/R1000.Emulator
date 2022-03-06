#ifndef R1000_XTAGRAM
#define R1000_XTAGRAM

#define DQPINS() \
	PIN(0, pin17) PIN(1, pin18) PIN(2, pin19) PIN(3, pin20) \
	PIN(4, pin21) PIN(5, pin22) PIN(6, pin23) PIN(7, pin24) \
	PIN(8, pin25) PIN(9, pin26) PIN(10, pin27) PIN(11, pin28) \
	PIN(12, pin29) PIN(13, pin30) PIN(14, pin31) PIN(15, pin32) \
	PIN(16, pin33) PIN(17, pin34) PIN(18, pin35) PIN(19, pin36) \
	PIN(20, pin37) PIN(21, pin38) PIN(22, pin39) PIN(23, pin40) \
	PIN(24, pin41) PIN(25, pin42) PIN(26, pin43) PIN(27, pin44) \
	PIN(28, pin45) PIN(29, pin46) PIN(30, pin47) PIN(31, pin48) \
	PIN(32, pin49) PIN(33, pin50) PIN(34, pin51) PIN(35, pin52) \
	PIN(36, pin53) PIN(37, pin54) PIN(38, pin55) PIN(39, pin56) \
	PIN(40, pin57) PIN(41, pin58) PIN(42, pin59) PIN(43, pin60) \
	PIN(44, pin61) PIN(45, pin62) PIN(46, pin63) PIN(47, pin64) \
	PIN(48, pin65) PIN(49, pin66) PIN(50, pin67) PIN(51, pin68) \
	PIN(52, pin69) PIN(53, pin70) PIN(54, pin71) PIN(55, pin72) \
	PIN(56, pin73) PIN(57, pin74) PIN(58, pin75) PIN(59, pin76) \
	PIN(60, pin77) PIN(61, pin78) PIN(62, pin79) PIN(63, pin80)

struct scm_xtagram_state;

SC_MODULE(SCM_XTAGRAM)
{
	sc_in <sc_logic>	pin1;	// WE_ (input)
	sc_in <sc_logic>	pin2;	// CS_ (input)
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
	sc_in <sc_logic>	pin13;	// A10 (input)
	sc_in <sc_logic>	pin14;	// A11 (input)
	sc_in <sc_logic>	pin15;	// A12 (input)
	sc_in <sc_logic>	pin16;	// A13 (input)

	#define PIN(bit, pin_no) sc_inout_resolved pin_no;
	DQPINS()
	#undef PIN

	SC_HAS_PROCESS(SCM_XTAGRAM);

	SCM_XTAGRAM(sc_module_name nm, const char *arg);

	private:
	struct scm_xtagram_state *state;
	void doit(void);
};

#endif /* R1000_XTAGRAM */
