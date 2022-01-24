#ifndef R1000_25S10
#define R1000_25S10

// Four-Bit Shifter with Three-State Outputs
// Am25s10 03611B

struct scm_25s10_state;

SC_MODULE(SCM_25S10)
{
	sc_in <sc_logic>	pin1;	// I-3 - I-3 (input)
	sc_in <sc_logic>	pin2;	// I-2 - I-2 (input)
	sc_in <sc_logic>	pin3;	// I-1 - I-1 (input)
	sc_in <sc_logic>	pin4;	// I0 - I0 (input)
	sc_in <sc_logic>	pin5;	// I1 - I1 (input)
	sc_in <sc_logic>	pin6;	// I2 - I2 (input)
	sc_in <sc_logic>	pin7;	// I3 - I3 (input)
	sc_in <sc_logic>	pin9;	// S1 - S0 (input)
	sc_in <sc_logic>	pin10;	// S0 - S1 (input)
	sc_out <sc_logic>	pin11;	// Y3 (tri_state)
	sc_out <sc_logic>	pin12;	// Y2 (tri_state)
	sc_in <sc_logic>	pin13;	// OE_ (input)
	sc_out <sc_logic>	pin14;	// Y1 (tri_state)
	sc_out <sc_logic>	pin15;	// Y0 (tri_state)

	SC_HAS_PROCESS(SCM_25S10);

	SCM_25S10(sc_module_name nm, const char *arg) :
		sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1 << pin2 << pin3 << pin4 << pin5 << pin6
			  << pin7 << pin9 << pin10 << pin13;
		loadit(arg);
	}
	private:
	struct scm_25s10_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_25S10 */
