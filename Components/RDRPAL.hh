// Generated from ../_Firmware/RDRGAL-02.BIN by gal_to_systemc.py
#ifndef R1000_RDRPAL
#define R1000_RDRPAL

struct scm_rdrpal_state;

SC_MODULE(SCM_RDRPAL)
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
	sc_in <sc_logic>	pin11;
	sc_in <sc_logic>	pin12;
	sc_in <sc_logic>	pin13;
	sc_in <sc_logic>	pin14;
	sc_in <sc_logic>	pin15;

	sc_out <sc_logic>	pin16;
	sc_out <sc_logic>	pin17;
	sc_out <sc_logic>	pin18;
	sc_out <sc_logic>	pin19;

	SC_HAS_PROCESS(SCM_RDRPAL);

	SCM_RDRPAL(sc_module_name nm, const char *arg);

	private:
	struct scm_rdrpal_state *state;
	void doit(void);
};

#endif /* R1000_RDRPAL */
