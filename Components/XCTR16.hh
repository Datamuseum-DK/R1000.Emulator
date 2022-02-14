#ifndef R1000_XCTR16
#define R1000_XCTR16

struct scm_xctr16_state;

#define XCTR16_PINS() \
	PIN(0, pin1, pin17) PIN(1, pin2, pin18) PIN(2, pin3, pin19) \
	PIN(3, pin4, pin20) PIN(4, pin5, pin21) PIN(5, pin6, pin22) \
	PIN(6, pin7, pin23) PIN(7, pin8, pin24) PIN(8, pin9, pin25) \
	PIN(9, pin10, pin26) PIN(10, pin11, pin27) PIN(11, pin12, pin28) \
	PIN(12, pin13, pin29) PIN(13, pin14, pin30) PIN(14, pin15, pin31) \
	PIN(15, pin16, pin32)


SC_MODULE(SCM_XCTR16)
{
	sc_in <sc_logic>	pin33;	// CLK (input)
	sc_in <sc_logic>	pin34;	// LD_ (input)
	sc_in <sc_logic>	pin35;	// UP (input)
	sc_in <sc_logic>	pin36;	// ENP_ (input)
	sc_in <sc_logic>	pin37;	// ENT_ (input)
	sc_out <sc_logic>	pin38;	// CO15_ (output)
	sc_out <sc_logic>	pin39;	// CO11_ (output)

	#define PIN(bit, pin_in, pin_out) \
	sc_in <sc_logic>	pin_in;
	XCTR16_PINS()
	#undef PIN

	#define PIN(bit, pin_in, pin_out) \
	sc_out <sc_logic>	pin_out;
	XCTR16_PINS()
	#undef PIN

	SC_HAS_PROCESS(SCM_XCTR16);

	SCM_XCTR16(sc_module_name nm, const char *arg);

	private:
	struct scm_xctr16_state *state;
	void doit(void);
};

#endif /* R1000_XCTR16 */
