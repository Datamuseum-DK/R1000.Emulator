#ifndef R1000_XMUXI16
#define R1000_XMUXI16

struct scm_xmuxi16_state;

#define XMUXI16_PINS() \
	PIN(0, pin3, pin19, pin35) \
	PIN(1, pin4, pin20, pin36) \
	PIN(2, pin5, pin21, pin37) \
	PIN(3, pin6, pin22, pin38) \
	PIN(4, pin7, pin23, pin39) \
	PIN(5, pin8, pin24, pin40) \
	PIN(6, pin9, pin25, pin41) \
	PIN(7, pin10, pin26, pin42) \
	PIN(8, pin11, pin27, pin43) \
	PIN(9, pin12, pin28, pin44) \
	PIN(10, pin13, pin29, pin45) \
	PIN(11, pin14, pin30, pin46) \
	PIN(12, pin15, pin31, pin47) \
	PIN(13, pin16, pin32, pin48) \
	PIN(14, pin17, pin33, pin49) \
	PIN(15, pin18, pin34, pin50)


SC_MODULE(SCM_XMUXI16)
{
	sc_in <sc_logic>	pin1;	// S
	sc_in <sc_logic>	pin2;	// OE

	#define PIN(bit, pin_a, pin_b, pin_out) \
	sc_in <sc_logic>	pin_a;
	XMUXI16_PINS()
	#undef PIN

	#define PIN(bit, pin_a, pin_b, pin_out) \
	sc_in <sc_logic>	pin_b;
	XMUXI16_PINS()
	#undef PIN

	#define PIN(bit, pin_a, pin_b, pin_out) \
	sc_out <sc_logic>	pin_out;
	XMUXI16_PINS()
	#undef PIN

	SC_HAS_PROCESS(SCM_XMUXI16);

	SCM_XMUXI16(sc_module_name nm, const char *arg);

	private:
	struct scm_xmuxi16_state *state;
	void doit(void);
};

#endif /* R1000_XMUXI16 */
