#ifndef R1000_8051
#define R1000_8051

struct scm_8051_state;

SC_MODULE(SCM_8051)
{
	sc_inout_resolved	pin1;	// P1.0 - D7 (tri_state)
	sc_inout_resolved	pin2;	// P1.1 - D6 (tri_state)
	sc_inout_resolved	pin3;	// P1.2 - D5 (tri_state)
	sc_inout_resolved	pin4;	// P1.3 - D4 (tri_state)
	sc_inout_resolved	pin5;	// P1.4 - D3 (tri_state)
	sc_inout_resolved	pin6;	// P1.5 - D2 (tri_state)
	sc_inout_resolved	pin7;	// P1.6 - D1 (tri_state)
	sc_inout_resolved	pin8;	// P1.7 - D0 (tri_state)
	sc_in <sc_logic>	pin9;	// RST - RST (input)
	sc_inout <sc_logic>	pin10;	// P3.0/RXD - RXD (input)
	sc_inout <sc_logic>	pin11;	// P3.1/TXD - TXD (output)
	sc_inout <sc_logic>	pin12;	// P3.2/INT0_ - INT0_ (input)
	sc_inout <sc_logic>	pin13;	// P3.3/INT1_ - INT1_ (input)
	sc_inout <sc_logic>	pin14;	// P3.4/TO - T0 (input)
	sc_inout <sc_logic>	pin15;	// P3.5/TI - T1 (input)
	sc_inout <sc_logic>	pin16;	// P3.6/WR_ - WR_ (output)
	sc_inout <sc_logic>	pin17;	// P3.7/RD_ - RD_ (output)
	sc_in <sc_logic>	pin18;	// XTAL2 - XTAL2 (input)
	sc_in <sc_logic>	pin19;	// XTAL1 - XTAL1 (input)
	sc_inout_resolved	pin21;	// P2.0 - D15 (tri_state)
	sc_inout_resolved	pin22;	// P2.1 - D14 (tri_state)
	sc_inout_resolved	pin23;	// P2.2 - D13 (tri_state)
	sc_inout_resolved	pin24;	// P2.3 - D12 (tri_state)
	sc_inout_resolved	pin25;	// P2.4 - D11 (tri_state)
	sc_inout_resolved	pin26;	// P2.5 - D10 (tri_state)
	sc_inout_resolved	pin27;	// P2.6 - D9 (tri_state)
	sc_inout_resolved	pin28;	// P2.7 - D8 (tri_state)
	sc_out <sc_logic>	pin29;	// PSEN_ - PSEN_ (output)
	sc_out <sc_logic>	pin30;	// ALE_ - ALE (output)
	sc_in <sc_logic>	pin31;	// EA_ - EA_ (input)
	sc_out <sc_logic>	pin32;	// P0.7 - C0 (output)
	sc_out <sc_logic>	pin33;	// P0.6 - C1 (output)
	sc_out <sc_logic>	pin34;	// P0.5 - C2 (output)
	sc_out <sc_logic>	pin35;	// P0.4 - C3 (output)
	sc_out <sc_logic>	pin36;	// P0.3 - C4 (output)
	sc_out <sc_logic>	pin37;	// P0.2 - C5 (output)
	sc_out <sc_logic>	pin38;	// P0.1 - C6 (output)
	sc_out <sc_logic>	pin39;	// P0.0 - C7 (output)

	SC_HAS_PROCESS(SCM_8051);

	SCM_8051(sc_module_name nm, const char *arg);

	private:
	struct scm_8051_state *state;
	struct diagproc_ctrl *diag_ctrl;
	unsigned cycle;
	void doit(void);
};

#endif /* R1000_8051 */
