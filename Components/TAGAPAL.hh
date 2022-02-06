// Generated from ../_Firmware/TAGAGAL-01.BIN by gal_to_systemc.py
#ifndef R1000_TAGAPAL
#define R1000_TAGAPAL

struct scm_tagapal_state;

SC_MODULE(SCM_TAGAPAL)
{
	sc_in <sc_logic>	pin1;
	sc_in <sc_logic>	pin2;
	sc_in <sc_logic>	pin3;
	sc_in <sc_logic>	pin4;
	sc_in <sc_logic>	pin5;
	sc_in <sc_logic>	pin6;
	sc_in <sc_logic>	pin7;
	sc_in <sc_logic>	pin8;
	sc_in <sc_logic>	pin9;
	sc_in <sc_logic>	pin10;
	sc_in <sc_logic>	pin11;
	sc_in <sc_logic>	pin13;
	sc_in <sc_logic>	pin14;
	sc_in <sc_logic>	pin15;
	sc_in <sc_logic>	pin20;
	sc_in <sc_logic>	pin21;
	sc_in <sc_logic>	pin22;
	sc_in <sc_logic>	pin23;

	sc_out <sc_logic>	pin16;
	sc_out <sc_logic>	pin17;
	sc_out <sc_logic>	pin18;
	sc_out <sc_logic>	pin19;

	SC_HAS_PROCESS(SCM_TAGAPAL);

	SCM_TAGAPAL(sc_module_name nm, const char *arg);

	private:
	struct scm_tagapal_state *state;
	void doit(void);
};

#endif /* R1000_TAGAPAL */
