#ifndef R1000_XSTKRAM
#define R1000_XSTKRAM

struct scm_xstkram_state;

#define XSTKRAM_PINS() \
	PIN(0, pin1, pin17) \
	PIN(1, pin2, pin18) \
	PIN(2, pin3, pin19) \
	PIN(3, pin4, pin20) \
	PIN(4, pin5, pin21) \
	PIN(5, pin6, pin22) \
	PIN(6, pin7, pin23) \
	PIN(7, pin8, pin24) \
	PIN(8, pin9, pin25) \
	PIN(9, pin10, pin26) \
	PIN(10, pin11, pin27) \
	PIN(11, pin12, pin28) \
	PIN(12, pin13, pin29) \
	PIN(13, pin14, pin30) \
	PIN(14, pin15, pin31) \
	PIN(15, pin16, pin32) \


SC_MODULE(SCM_XSTKRAM)
{

	#define PIN(bit, pin_in, pin_out) \
	sc_in <sc_logic>	pin_in;
	XSTKRAM_PINS()
	#undef PIN

	#define PIN(bit, pin_in, pin_out) \
	sc_out <sc_logic>	pin_out;
	XSTKRAM_PINS()
	#undef PIN

	sc_in <sc_logic>	pin33;	// WE
	sc_in <sc_logic>	pin34;	// CS
	sc_in <sc_logic>	pin35;	// A0
	sc_in <sc_logic>	pin36;	// A1
	sc_in <sc_logic>	pin37;	// A2
	sc_in <sc_logic>	pin38;	// A3

	SC_HAS_PROCESS(SCM_XSTKRAM);

	SCM_XSTKRAM(sc_module_name nm, const char *arg);

	private:
	struct scm_xstkram_state *state;
	void doit(void);
};

#endif /* R1000_XSTKRAM */
