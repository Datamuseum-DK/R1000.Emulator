#ifndef R1000_F579
#define R1000_F579

// 8-bit bidirectional binary counter (3-State)
// Philips 2000 Dec 18 Supersedes data of 1992 May 04


struct scm_f579_state;

SC_MODULE(SCM_F579)
{
	sc_in <sc_logic>	pin1;	// CP - CLK (input)
	sc_inout_resolved	pin2;	// I/O0 - IO7 (tri_state)
	sc_inout_resolved	pin3;	// I/O1 - IO6 (tri_state)
	sc_inout_resolved	pin4;	// I/O2 - IO5 (tri_state)
	sc_inout_resolved	pin5;	// I/O3 - IO4 (tri_state)
	sc_inout_resolved	pin7;	// I/O4 - IO3 (tri_state)
	sc_inout_resolved	pin8;	// I/O5 - IO2 (tri_state)
	sc_inout_resolved	pin9;	// I/O6 - IO1 (tri_state)
	sc_inout_resolved	pin10;	// I/O7 - IO0 (tri_state)
	sc_in <sc_logic>	pin11;	// OE_ - OE_ (input)
	sc_in <sc_logic>	pin12;	// CS_ - CS_ (input)
	sc_in <sc_logic>	pin13;	// PE_ - LD_ (input)
	sc_in <sc_logic>	pin14;	// U/D_ - U/B_ (input)
	sc_out <sc_logic>	pin15;	// TC_ - C0_ (output)
	sc_in <sc_logic>	pin17;	// CET_ - CET_ (input)
	sc_in <sc_logic>	pin18;	// CEP_ - CEP_ (input)
	sc_in <sc_logic>	pin19;	// SR_ - SR_ (input)
	sc_in <sc_logic>	pin20;	// MR_ - MR_ (input)

	SC_HAS_PROCESS(SCM_F579);

	SCM_F579(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin1.pos() << pin20 << pin11 << pin12 << pin13 << pin17 << pin18;
		loadit(arg);
	}

	private:
	struct scm_f579_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_F579 */
