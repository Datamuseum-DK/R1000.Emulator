#ifndef R1000_2661B
#define R1000_2661B

struct scm_2661B_state;

SC_MODULE(SCM_2661B)
{
	sc_inout_resolved	pin1;	// D5 (tri_state)
	sc_inout_resolved	pin2;	// D4 (tri_state)
	sc_in <sc_logic>	pin3;	// RXD (input)
	sc_inout_resolved	pin5;	// D3 (tri_state)
	sc_inout_resolved	pin6;	// D2 (tri_state)
	sc_inout_resolved	pin7;	// D1 (tri_state)
	sc_inout_resolved	pin8;	// D0 (tri_state)
	sc_in <sc_logic>	pin9;	// XSYNC (input)
	sc_in <sc_logic>	pin10;	// A0 (input)
	sc_in <sc_logic>	pin11;	// CE_ (input)
	sc_in <sc_logic>	pin12;	// A1 (input)
	sc_in <sc_logic>	pin13;	// R_/W (input)
	sc_out <sc_logic>	pin14;	// RXRDY_ (output)
	sc_out <sc_logic>	pin15;	// TXRDY_ (output)
	sc_in <sc_logic>	pin16;	// DCD_ (input)
	sc_in <sc_logic>	pin17;	// CTS_ (input)
	sc_out <sc_logic>	pin18;	// TXEMT_ (output)
	sc_out <sc_logic>	pin19;	// TXD (output)
	sc_in <sc_logic>	pin20;	// BRCLK (input)
	sc_in <sc_logic>	pin21;	// RESET (input)
	sc_in <sc_logic>	pin22;	// DSR_ (input)
	sc_out <sc_logic>	pin23;	// RTS_ (output)
	sc_out <sc_logic>	pin24;	// DTR_ (output)
	sc_out <sc_logic>	pin25;	// BRKDET (output)
	sc_inout_resolved	pin27;	// D7 (tri_state)
	sc_inout_resolved	pin28;	// D6 (tri_state)

	SC_HAS_PROCESS(SCM_2661B);

	SCM_2661B(sc_module_name nm, const char *arg) : sc_module(nm)
	{
		cout << "MISSING SCM_2661B IMPLEMENTATION\n";
		SC_METHOD(doit);
		loadit(arg);
	}

	private:
	struct scm_2661B_state *state;
	void loadit(const char *arg);
	void doit(void);
};

#endif /* R1000_2661B */
