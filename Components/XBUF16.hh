#ifndef R1000_XBUF16
#define R1000_XBUF16

struct scm_xbuf16_state;

#define XBUF16_PINS() \
	PIN(0, pin2, pin18) PIN(1, pin3, pin19) PIN(2, pin4, pin20) \
	PIN(3, pin5, pin21) PIN(4, pin6, pin22) PIN(5, pin7, pin23) \
	PIN(6, pin8, pin24) PIN(7, pin9, pin25) PIN(8, pin10, pin26) \
	PIN(9, pin11, pin27) PIN(10, pin12, pin28) PIN(11, pin13, pin29) \
	PIN(12, pin14, pin30) PIN(13, pin15, pin31) PIN(14, pin16, pin32) \
	PIN(15, pin17, pin33)

SC_MODULE(SCM_XBUF16)
{
	sc_in <sc_logic>	pin1;	// OE_ (input)

	#define PIN(bit, pin_in, pin_out) \
	sc_in <sc_logic>        pin_in;
	XBUF16_PINS()
	#undef PIN

	#define PIN(bit, pin_in, pin_out) \
	sc_out <sc_logic>       pin_out;
	XBUF16_PINS()
	#undef PIN

	SC_HAS_PROCESS(SCM_XBUF16);

	SCM_XBUF16(sc_module_name nm, const char *arg);

	private:
	struct scm_xbuf16_state *state;
	void doit(void);
};

#endif /* R1000_XBUF16 */
