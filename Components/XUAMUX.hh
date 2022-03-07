#ifndef R1000_XUAMUX
#define R1000_XUAMUX

struct scm_xuamux_state;

#define XUAMUX_PINS() \
	PIN(0, pin1, pin15, pin29, pin43, pin57, pin71) \
	PIN(1, pin2, pin16, pin30, pin44, pin58, pin72) \
	PIN(2, pin3, pin17, pin31, pin45, pin59, pin73) \
	PIN(3, pin4, pin18, pin32, pin46, pin60, pin74) \
	PIN(4, pin5, pin19, pin33, pin47, pin61, pin75) \
	PIN(5, pin6, pin20, pin34, pin48, pin62, pin76) \
	PIN(6, pin7, pin21, pin35, pin49, pin63, pin77) \
	PIN(7, pin8, pin22, pin36, pin50, pin64, pin78) \
	PIN(8, pin9, pin23, pin37, pin51, pin65, pin79) \
	PIN(9, pin10, pin24, pin38, pin52, pin66, pin80) \
	PIN(10, pin11, pin25, pin39, pin53, pin67, pin81) \
	PIN(11, pin12, pin26, pin40, pin54, pin68, pin82) \
	PIN(12, pin13, pin27, pin41, pin55, pin69, pin83) \
	PIN(13, pin14, pin28, pin42, pin56, pin70, pin84)

SC_MODULE(SCM_XUAMUX)
{
	#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) \
	sc_in <sc_logic>	pin_a;
	XUAMUX_PINS()
	#undef PIN

	#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) \
	sc_in <sc_logic>	pin_b;
	XUAMUX_PINS()
	#undef PIN

	#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) \
	sc_in <sc_logic>	pin_c;
	XUAMUX_PINS()
	#undef PIN

	#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) \
	sc_in <sc_logic>	pin_de;
	XUAMUX_PINS()
	#undef PIN

	#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) \
	sc_in <sc_logic>	pin_fgh;
	XUAMUX_PINS()
	#undef PIN

	#define PIN(bit, pin_a, pin_b, pin_c, pin_de, pin_fgh, pin_q) \
	sc_out <sc_logic>	pin_q;
	XUAMUX_PINS()
	#undef PIN

	sc_in <sc_logic>	pin85;	// S0
	sc_in <sc_logic>	pin86;	// S1
	sc_in <sc_logic>	pin87;	// S2
	sc_in <sc_logic>	pin88;	// OE

	SC_HAS_PROCESS(SCM_XUAMUX);

	SCM_XUAMUX(sc_module_name nm, const char *arg);

	private:
	struct scm_xuamux_state *state;
	void doit(void);
};

#endif /* R1000_XUAMUX */
