#ifndef R1000_XREG64
#define R1000_XREG64

struct scm_XREG64_state;

#define XREG64_PINS() \
	PIN(0, pin3, pin67) PIN(1, pin4, pin68) PIN(2, pin5, pin69) \
	PIN(3, pin6, pin70) PIN(4, pin7, pin71) PIN(5, pin8, pin72) \
	PIN(6, pin9, pin73) PIN(7, pin10, pin74) PIN(8, pin11, pin75) \
	PIN(9, pin12, pin76) PIN(10, pin13, pin77) PIN(11, pin14, pin78) \
	PIN(12, pin15, pin79) PIN(13, pin16, pin80) PIN(14, pin17, pin81) \
	PIN(15, pin18, pin82) PIN(16, pin19, pin83) PIN(17, pin20, pin84) \
	PIN(18, pin21, pin85) PIN(19, pin22, pin86) PIN(20, pin23, pin87) \
	PIN(21, pin24, pin88) PIN(22, pin25, pin89) PIN(23, pin26, pin90) \
	PIN(24, pin27, pin91) PIN(25, pin28, pin92) PIN(26, pin29, pin93) \
	PIN(27, pin30, pin94) PIN(28, pin31, pin95) PIN(29, pin32, pin96) \
	PIN(30, pin33, pin97) PIN(31, pin34, pin98) PIN(32, pin35, pin99) \
	PIN(33, pin36, pin100) PIN(34, pin37, pin101) PIN(35, pin38, pin102) \
	PIN(36, pin39, pin103) PIN(37, pin40, pin104) PIN(38, pin41, pin105) \
	PIN(39, pin42, pin106) PIN(40, pin43, pin107) PIN(41, pin44, pin108) \
	PIN(42, pin45, pin109) PIN(43, pin46, pin110) PIN(44, pin47, pin111) \
	PIN(45, pin48, pin112) PIN(46, pin49, pin113) PIN(47, pin50, pin114) \
	PIN(48, pin51, pin115) PIN(49, pin52, pin116) PIN(50, pin53, pin117) \
	PIN(51, pin54, pin118) PIN(52, pin55, pin119) PIN(53, pin56, pin120) \
	PIN(54, pin57, pin121) PIN(55, pin58, pin122) PIN(56, pin59, pin123) \
	PIN(57, pin60, pin124) PIN(58, pin61, pin125) PIN(59, pin62, pin126) \
	PIN(60, pin63, pin127) PIN(61, pin64, pin128) PIN(62, pin65, pin129) \
	PIN(63, pin66, pin130)


SC_MODULE(SCM_XREG64)
{
	sc_in <sc_logic>	pin1;	// CLK_ (input)
	sc_in <sc_logic>	pin2;	// OE_ (input)

	#define PIN(bit, pin_in, pin_out) \
	sc_in <sc_logic>	pin_in;
	XREG64_PINS()
	#undef PIN

	#define PIN(bit, pin_in, pin_out) \
	sc_out <sc_logic>	pin_out;
	XREG64_PINS()
	#undef PIN

	SC_HAS_PROCESS(SCM_XREG64);

	SCM_XREG64(sc_module_name nm, const char *arg);

	private:
	struct scm_XREG64_state *state;
	void doit(void);
};

#endif /* R1000_XREG64 */
