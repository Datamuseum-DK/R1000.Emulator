#ifndef R1000_XREG16
#define R1000_XREG16

struct scm_xreg16_state;

#define XREG16_PINS() \
	PIN(0, pin3, pin19) PIN(1, pin4, pin20) PIN(2, pin5, pin21) \
	PIN(3, pin6, pin22) PIN(4, pin7, pin23) PIN(5, pin8, pin24) \
	PIN(6, pin9, pin25) PIN(7, pin10, pin26) PIN(8, pin11, pin27) \
	PIN(9, pin12, pin28) PIN(10, pin13, pin29) PIN(11, pin14, pin30) \
	PIN(12, pin15, pin31) PIN(13, pin16, pin32) PIN(14, pin17, pin33) \
	PIN(15, pin18, pin34)


SC_MODULE(SCM_XREG16)
{
	sc_in <sc_logic>	pin1;	// CLK_ (input)
	sc_in <sc_logic>	pin2;	// OE_ (input)

	#define PIN(bit, pin_in, pin_out) \
	sc_in <sc_logic>	pin_in;
	XREG16_PINS()
	#undef PIN

	#define PIN(bit, pin_in, pin_out) \
	sc_out <sc_logic>	pin_out;
	XREG16_PINS()
	#undef PIN

	SC_HAS_PROCESS(SCM_XREG16);

	SCM_XREG16(sc_module_name nm, const char *arg);

	private:
	struct scm_xreg16_state *state;
	void doit(void);
};

#endif /* R1000_XREG16 */
