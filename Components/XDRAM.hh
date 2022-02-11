#ifndef R1000_XDRAM
#define R1000_XDRAM

#define DQPINS() \
	PIN(0, pin13) PIN(1, pin14) PIN(2, pin15) PIN(3, pin16) \
	PIN(4, pin17) PIN(5, pin18) PIN(6, pin19) PIN(7, pin20) \
	PIN(8, pin21) PIN(9, pin22) PIN(10, pin23) PIN(11, pin24) \
	PIN(12, pin25) PIN(13, pin26) PIN(14, pin27) PIN(15, pin28) \
	PIN(16, pin29) PIN(17, pin30) PIN(18, pin31) PIN(19, pin32) \
	PIN(20, pin33) PIN(21, pin34) PIN(22, pin35) PIN(23, pin36) \
	PIN(24, pin37) PIN(25, pin38) PIN(26, pin39) PIN(27, pin40) \
	PIN(28, pin41) PIN(29, pin42) PIN(30, pin43) PIN(31, pin44) \
	PIN(32, pin45) PIN(33, pin46) PIN(34, pin47) PIN(35, pin48) \
	PIN(36, pin49) PIN(37, pin50) PIN(38, pin51) PIN(39, pin52) \
	PIN(40, pin53) PIN(41, pin54) PIN(42, pin55) PIN(43, pin56) \
	PIN(44, pin57) PIN(45, pin58) PIN(46, pin59) PIN(47, pin60) \
	PIN(48, pin61) PIN(49, pin62) PIN(50, pin63) PIN(51, pin64) \
	PIN(52, pin65) PIN(53, pin66) PIN(54, pin67) PIN(55, pin68) \
	PIN(56, pin69) PIN(57, pin70) PIN(58, pin71) PIN(59, pin72) \
	PIN(60, pin73) PIN(61, pin74) PIN(62, pin75) PIN(63, pin76)

struct scm_xdram_state;

SC_MODULE(SCM_XDRAM)
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
	DQPINS()
	#undef PIN
	sc_in <sc_logic>	pin77;	// RAS (input)

	SC_HAS_PROCESS(SCM_XDRAM);

	SCM_XDRAM(sc_module_name nm, const char *arg);

	private:
	struct scm_xdram_state *state;
	void doit(void);
};

#endif /* R1000_XDRAM */
