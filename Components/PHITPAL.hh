// Generated from ../_Firmware/PHITGAL-02.BIN by gal_to_systemc.py
#ifndef R1000_PHITPAL
#define R1000_PHITPAL

struct scm_phitpal_state;

SC_MODULE(SCM_PHITPAL)
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

	sc_out <sc_logic>	pin12;
	sc_out <sc_logic>	pin13;
	sc_out <sc_logic>	pin14;
	sc_out <sc_logic>	pin15;
	sc_out <sc_logic>	pin16;
	sc_out <sc_logic>	pin17;
	sc_out <sc_logic>	pin18;
	sc_out <sc_logic>	pin19;

	SC_HAS_PROCESS(SCM_PHITPAL);

	SCM_PHITPAL(sc_module_name nm, const char *arg);

	private:
	struct scm_phitpal_state *state;
	void doit(void);
};

#endif /* R1000_PHITPAL */
