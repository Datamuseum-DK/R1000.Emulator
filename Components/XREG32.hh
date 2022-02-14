#ifndef R1000_XREG32
#define R1000_XREG32

struct scm_xreg32_state;

#define XREG32_PINS() \
	PIN(0, pin3, pin35) PIN(1, pin4, pin36) PIN(2, pin5, pin37) \
	PIN(3, pin6, pin38) PIN(4, pin7, pin39) PIN(5, pin8, pin40) \
	PIN(6, pin9, pin41) PIN(7, pin10, pin42) PIN(8, pin11, pin43) \
	PIN(9, pin12, pin44) PIN(10, pin13, pin45) PIN(11, pin14, pin46) \
	PIN(12, pin15, pin47) PIN(13, pin16, pin48) PIN(14, pin17, pin49) \
	PIN(15, pin18, pin50) PIN(16, pin19, pin51) PIN(17, pin20, pin52) \
	PIN(18, pin21, pin53) PIN(19, pin22, pin54) PIN(20, pin23, pin55) \
	PIN(21, pin24, pin56) PIN(22, pin25, pin57) PIN(23, pin26, pin58) \
	PIN(24, pin27, pin59) PIN(25, pin28, pin60) PIN(26, pin29, pin61) \
	PIN(27, pin30, pin62) PIN(28, pin31, pin63) PIN(29, pin32, pin64) \
	PIN(30, pin33, pin65) PIN(31, pin34, pin66)


SC_MODULE(SCM_XREG32)
{
	sc_in <sc_logic>	pin1;	// CLK_ (input)
	sc_in <sc_logic>	pin2;	// OE_ (input)

	#define PIN(bit, pin_in, pin_out) \
	sc_in <sc_logic>	pin_in;
	XREG32_PINS()
	#undef PIN

	#define PIN(bit, pin_in, pin_out) \
	sc_out <sc_logic>	pin_out;
	XREG32_PINS()
	#undef PIN

	SC_HAS_PROCESS(SCM_XREG32);

	SCM_XREG32(sc_module_name nm, const char *arg);

	private:
	struct scm_xreg32_state *state;
	void doit(void);
};

#endif /* R1000_XREG32 */
