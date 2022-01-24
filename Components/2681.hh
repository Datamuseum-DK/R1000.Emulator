#ifndef R1000_2681
#define R1000_2681

struct scm_2681_state;

SC_MODULE(SCM_2681)
{
	sc_in <sc_logic>	pin1;	// A3 (input)
	sc_in <sc_logic>	pin2;	// DCDA_ (input)
	sc_in <sc_logic>	pin3;	// A2 (input)
	sc_in <sc_logic>	pin4;	// DSRA_ (input)
	sc_in <sc_logic>	pin5;	// A1 (input)
	sc_in <sc_logic>	pin6;	// A0 (input)
	sc_in <sc_logic>	pin7;	// CTSA_ (input)
	sc_in <sc_logic>	pin8;	// WDN_ (input)
	sc_in <sc_logic>	pin9;	// RDN_ (input)
	sc_in <sc_logic>	pin10;	// RXDB (input)
	sc_out <sc_logic>	pin11;	// TXDB (output)
	sc_out <sc_logic>	pin12;	// DTRA_ (output)
	sc_out <sc_logic>	pin13;	// PITINT_ (output)
	sc_out <sc_logic>	pin14;	// RXRDYB_ (output)
	sc_out <sc_logic>	pin15;	// TXRDYB_ (output)
	sc_inout_resolved	pin16;	// D6 (tri_state)
	sc_inout_resolved	pin17;	// D4 (tri_state)
	sc_inout_resolved	pin18;	// D2 (tri_state)
	sc_inout_resolved	pin19;	// D0 (tri_state)
	sc_out <sc_logic>	pin21;	// BSCHGA_ (output)
	sc_inout_resolved	pin22;	// D1 (tri_state)
	sc_inout_resolved	pin23;	// D3 (tri_state)
	sc_inout_resolved	pin24;	// D5 (tri_state)
	sc_inout_resolved	pin25;	// D7 (tri_state)
	sc_out <sc_logic>	pin26;	// TXRDYA_ (output)
	sc_out <sc_logic>	pin27;	// RXRDYA_ (output)
	sc_out <sc_logic>	pin28;	// OP2_ (output)
	sc_out <sc_logic>	pin29;	// RTSA_ (output)
	sc_out <sc_logic>	pin30;	// TXDA (output)
	sc_in <sc_logic>	pin31;	// RXDA (input)
	sc_in <sc_logic>	pin32;	// CLK (input)
	sc_in <sc_logic>	pin33;	// X2 (input)
	sc_in <sc_logic>	pin34;	// RESET (input)
	sc_in <sc_logic>	pin35;	// CEN_ (input)
	sc_in <sc_logic>	pin36;	// PITCLK (input)
	sc_in <sc_logic>	pin37;	// RXCB (input)
	sc_in <sc_logic>	pin38;	// TXCB (input)
	sc_in <sc_logic>	pin39;	// IP4 (input)

	SC_HAS_PROCESS(SCM_2681);

	SCM_2681(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		SC_METHOD(doit);
		sensitive << pin36.pos();
		loadit(arg);
	}

	private:
	struct scm_2681_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_2681 */
