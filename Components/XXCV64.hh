#ifndef R1000_XXCV64
#define R1000_XXCV64

struct scm_xxcv64_state;

#define XXCV64_LPIN 63

#define XXCV64_PINS() \
	PIN(0, pin1, pin65) PIN(1, pin2, pin66) PIN(2, pin3, pin67) \
	PIN(3, pin4, pin68) PIN(4, pin5, pin69) PIN(5, pin6, pin70) \
	PIN(6, pin7, pin71) PIN(7, pin8, pin72) PIN(8, pin9, pin73) \
	PIN(9, pin10, pin74) PIN(10, pin11, pin75) PIN(11, pin12, pin76) \
	PIN(12, pin13, pin77) PIN(13, pin14, pin78) PIN(14, pin15, pin79) \
	PIN(15, pin16, pin80) PIN(16, pin17, pin81) PIN(17, pin18, pin82) \
	PIN(18, pin19, pin83) PIN(19, pin20, pin84) PIN(20, pin21, pin85) \
	PIN(21, pin22, pin86) PIN(22, pin23, pin87) PIN(23, pin24, pin88) \
	PIN(24, pin25, pin89) PIN(25, pin26, pin90) PIN(26, pin27, pin91) \
	PIN(27, pin28, pin92) PIN(28, pin29, pin93) PIN(29, pin30, pin94) \
	PIN(30, pin31, pin95) PIN(31, pin32, pin96) PIN(32, pin33, pin97) \
	PIN(33, pin34, pin98) PIN(34, pin35, pin99) PIN(35, pin36, pin100) \
	PIN(36, pin37, pin101) PIN(37, pin38, pin102) PIN(38, pin39, pin103) \
	PIN(39, pin40, pin104) PIN(40, pin41, pin105) PIN(41, pin42, pin106) \
	PIN(42, pin43, pin107) PIN(43, pin44, pin108) PIN(44, pin45, pin109) \
	PIN(45, pin46, pin110) PIN(46, pin47, pin111) PIN(47, pin48, pin112) \
	PIN(48, pin49, pin113) PIN(49, pin50, pin114) PIN(50, pin51, pin115) \
	PIN(51, pin52, pin116) PIN(52, pin53, pin117) PIN(53, pin54, pin118) \
	PIN(54, pin55, pin119) PIN(55, pin56, pin120) PIN(56, pin57, pin121) \
	PIN(57, pin58, pin122) PIN(58, pin59, pin123) PIN(59, pin60, pin124) \
	PIN(60, pin61, pin125) PIN(61, pin62, pin126) PIN(62, pin63, pin127) \
	PIN(63, pin64, pin128)

#define XXCV64_OEA pin129
#define XXCV64_CBA pin130
#define XXCV64_SBA pin131
#define XXCV64_OEB pin132
#define XXCV64_CAB pin133
#define XXCV64_SAB pin134

SC_MODULE(SCM_XXCV64)
{

	#define PIN(bit, pin_a, pin_b) \
	sc_inout_resolved        pin_a;
	XXCV64_PINS()
	#undef PIN

	#define PIN(bit, pin_a, pin_b) \
	sc_inout_resolved       pin_b;
	XXCV64_PINS()
	#undef PIN

	sc_in <sc_logic>	XXCV64_OEA;
	sc_in <sc_logic>	XXCV64_CBA;
	sc_in <sc_logic>	XXCV64_SBA;
	sc_in <sc_logic>	XXCV64_OEB;
	sc_in <sc_logic>	XXCV64_CAB;
	sc_in <sc_logic>	XXCV64_SAB;

	SC_HAS_PROCESS(SCM_XXCV64);

	SCM_XXCV64(sc_module_name nm, const char *arg);

	private:
	struct scm_xxcv64_state *state;
	void doit(void);
};

#endif /* R1000_XXCV64 */
