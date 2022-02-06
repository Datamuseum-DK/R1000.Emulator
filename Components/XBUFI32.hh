#ifndef R1000_XBUFI32
#define R1000_XBUFI32

struct scm_xbufi32_state;

#define XBUFI32_PINS() \
	PIN(0, pin2, pin35) PIN(1, pin3, pin36) PIN(2, pin4, pin37) \
	PIN(3, pin5, pin38) PIN(4, pin6, pin39) PIN(5, pin7, pin40) \
	PIN(6, pin8, pin41) PIN(7, pin9, pin42) PIN(8, pin10, pin43) \
	PIN(9, pin11, pin44) PIN(10, pin12, pin45) PIN(11, pin13, pin46) \
	PIN(12, pin14, pin47) PIN(13, pin15, pin48) PIN(14, pin16, pin49) \
	PIN(15, pin17, pin50) PIN(16, pin18, pin51) PIN(17, pin19, pin52) \
	PIN(18, pin20, pin53) PIN(19, pin21, pin54) PIN(20, pin22, pin55) \
	PIN(21, pin23, pin56) PIN(22, pin24, pin57) PIN(23, pin25, pin58) \
	PIN(24, pin26, pin59) PIN(25, pin27, pin60) PIN(26, pin28, pin61) \
	PIN(27, pin29, pin62) PIN(28, pin30, pin63) PIN(29, pin31, pin64) \
	PIN(30, pin32, pin65) PIN(31, pin33, pin66)

SC_MODULE(SCM_XBUFI32)
{
	sc_in <sc_logic>	pin1;	// OE_ (input)

	#define PIN(bit, pin_in, pin_out) \
	sc_in <sc_logic>        pin_in;
	XBUFI32_PINS()
	#undef PIN

	#define PIN(bit, pin_in, pin_out) \
	sc_out <sc_logic>       pin_out;
	XBUFI32_PINS()
	#undef PIN

	SC_HAS_PROCESS(SCM_XBUFI32);

	SCM_XBUFI32(sc_module_name nm, const char *arg);

	private:
	struct scm_xbufi32_state *state;
	void doit(void);
};

#endif /* R1000_XBUFI32 */
