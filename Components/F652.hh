#ifndef R1000_F652
#define R1000_F652

struct scm_F652_state;

#define F652_LPIN 8

#define F652_PINS() \
	PIN(0, pin4, pin20) PIN(1, pin5, pin19) PIN(2, pin6, pin18) \
	PIN(3, pin7, pin17) PIN(4, pin8, pin16) PIN(5, pin9, pin15) \
	PIN(6, pin10, pin14) PIN(7, pin11, pin13)

#define F652_OEA pin21
#define F652_CBA pin23
#define F652_SBA pin22
#define F652_OEB pin3
#define F652_CAB pin1
#define F652_SAB pin2

SC_MODULE(SCM_F652)
{
	sc_in <sc_logic>	pin1;	// CAB (input)
	sc_in <sc_logic>	pin2;	// SAB (input)
	sc_in <sc_logic>	pin3;	// OEB_ (input)
	sc_inout_resolved	pin4;	// A0 (tri_state)
	sc_inout_resolved	pin5;	// A1 (tri_state)
	sc_inout_resolved	pin6;	// A2 (tri_state)
	sc_inout_resolved	pin7;	// A3 (tri_state)
	sc_inout_resolved	pin8;	// A4 (tri_state)
	sc_inout_resolved	pin9;	// A5 (tri_state)
	sc_inout_resolved	pin10;	// A6 (tri_state)
	sc_inout_resolved	pin11;	// A7 (tri_state)
	sc_inout_resolved	pin13;	// B7 (tri_state)
	sc_inout_resolved	pin14;	// B6 (tri_state)
	sc_inout_resolved	pin15;	// B5 (tri_state)
	sc_inout_resolved	pin16;	// B4 (tri_state)
	sc_inout_resolved	pin17;	// B3 (tri_state)
	sc_inout_resolved	pin18;	// B2 (tri_state)
	sc_inout_resolved	pin19;	// B1 (tri_state)
	sc_inout_resolved	pin20;	// B0 (tri_state)
	sc_in <sc_logic>	pin21;	// OEA_ (input)
	sc_in <sc_logic>	pin22;	// SBA (input)
	sc_in <sc_logic>	pin23;	// CBA (input)

	SC_HAS_PROCESS(SCM_F652);

	SCM_F652(sc_module_name nm, const char *arg);

	private:
	struct scm_F652_state *state;
	void doit(void);
};

#endif /* R1000_F652 */
