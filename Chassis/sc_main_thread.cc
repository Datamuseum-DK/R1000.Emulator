#include <stdio.h>
#include <systemc.h>

#include "Chassis/r1000sc.h"
#include "Chassis/r1000sc_priv.h"


double
sc_now(void)
{
	return (sc_time_stamp().to_double());
}

#include "Chassis/planes.hh"

#include "fiu_board_pub.hh"
#include "ioc_board_pub.hh"
#include "mem32_board_pub.hh"
#include "seq_board_pub.hh"
#include "typ_board_pub.hh"
#include "val_board_pub.hh"

static char Fiu_how[] = "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
static char Ioc_how[] = "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
static char Mem32_how[] = "+++++++++++++++++++++++++++++++++++++++++++++++";
static char Seq_how[] = "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
static char Typ_how[] = "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
static char Val_how[] = "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";


SC_MODULE(BackPlaneClocks)
{
	sc_out <sc_logic> bp_clk_2x;
	sc_out <sc_logic> bp_clk_2x_;
	sc_out <sc_logic> bp_phase_2x;
	sc_out <sc_logic> clk_2xe;
	sc_out <sc_logic> clk_2xe_;
	sc_out <sc_logic> clk_2x;
	sc_out <sc_logic> clk_2x_;
	sc_out <sc_logic> clk_h1_phd;
	sc_out <sc_logic> clk_h2_phd;
	sc_out <sc_logic> clk_h1e;
	sc_out <sc_logic> clk_h2e;
	sc_out <sc_logic> clk_h1;
	sc_out <sc_logic> clk_h2;
	sc_out <sc_logic> clk_q1_;
	sc_out <sc_logic> clk_q2_;
	sc_out <sc_logic> clk_q3_;
	sc_out <sc_logic> clk_q4_;
	unsigned pit = 0;
	unsigned now, when = 0;

	SC_CTOR(BackPlaneClocks)
	{
		SC_METHOD(doit);
	}

	void doit()
	{
		now = when;
		switch (now) {
		case 0:
			if (++pit == 32) {
				pit_clock();
				pit = 0;
			}
			bp_clk_2x = sc_logic_1; bp_clk_2x_ = sc_logic_0;
			bp_phase_2x = sc_logic_0;
			when = 5;
			break;
		case 5:
			clk_2xe = sc_logic_1; clk_2xe_ = sc_logic_0;
			when = 10;
			break;
		case 10:
			clk_2x = sc_logic_1; clk_2x_ = sc_logic_0;
			clk_h1e = sc_logic_1; clk_h2e = sc_logic_0;
			clk_q4_ = sc_logic_1; clk_q1_ = sc_logic_0;
			when = 20;
			break;
		case 20:
			clk_h1 = sc_logic_1; clk_h2 = sc_logic_0;
			when = 50;
			break;
		case 50:
			bp_clk_2x = sc_logic_0; bp_clk_2x_ = sc_logic_1;
			when = 55;
			break;
		case 55:
			clk_2xe = sc_logic_0; clk_2xe_ = sc_logic_1;
			when = 60;
			break;
		case 60:
			clk_2x = sc_logic_0; clk_2x_ = sc_logic_1;
			clk_h1_phd = sc_logic_1; clk_h2_phd = sc_logic_0;
			clk_q1_ = sc_logic_1; clk_q2_ = sc_logic_0;
			when = 100;
			break;
		case 100:
			bp_clk_2x = sc_logic_1; bp_clk_2x_ = sc_logic_0;
			bp_phase_2x = sc_logic_1;
			when = 105;
			break;
		case 105:
			clk_2xe = sc_logic_1; clk_2xe_ = sc_logic_0;
			when = 110;
			break;
		case 110:
			clk_2x = sc_logic_1; clk_2x_ = sc_logic_0;
			clk_h1e = sc_logic_0; clk_h2e = sc_logic_1;
			clk_q2_ = sc_logic_1; clk_q3_ = sc_logic_0;
			when = 120;
			break;
		case 120:
			clk_h1 = sc_logic_0; clk_h2 = sc_logic_1;
			when = 150;
			break;
		case 150:
			bp_clk_2x = sc_logic_0; bp_clk_2x_ = sc_logic_1;
			when = 155;
			break;
		case 155:
			clk_2xe = sc_logic_0; clk_2xe_ = sc_logic_1;
			when = 160;
			break;
		case 160:
			clk_2x = sc_logic_0; clk_2x_ = sc_logic_1;
			clk_h1_phd = sc_logic_0; clk_h2_phd = sc_logic_1;
			clk_q3_ = sc_logic_1; clk_q4_ = sc_logic_0;
			when = 200;
		}
		next_trigger((when - now) % 200, SC_NS);
		when = when % 200;
	}
};


SC_MODULE(PowerSequencer)
{
	sc_out <sc_logic> clamp;	// CLAMP
	sc_out <sc_logic> reset;	// RESET
	int do_trace = 0;

	SC_CTOR(PowerSequencer)
	{
		SC_THREAD(thread);
	}

	void thread()
	{
		clamp = sc_logic_0;
		reset = sc_logic_0;
		wait(100, SC_NS);
		clamp = sc_logic_1;
		wait(200, SC_NS);
		reset = sc_logic_Z;
	}
};

extern "C"
void *
sc_main_thread(void *priv)
{
	int argc = 0;
	char **argv = NULL;
	int retval;

	(void)priv;

	retval = sc_core::sc_elab_and_sim( argc, argv);
	fprintf(stderr, "HERE %s %d: retval %d\n", __func__, __LINE__, retval);
	return (NULL);
}

int
sc_main(int argc, char *argv[])
{
	mod_mem32 *mem32_0;
	mod_mem32 *mem32_2;
	mod_seq *seq;
	mod_typ *typ;
	mod_val *val;
	mod_fiu *fiu;
	mod_ioc *ioc;

	(void)argc;
	(void)argv;

	mod_planes planes("PLANES");
	planes.tf = sc_create_vcd_trace_file(tracefilename);

	// Order as seen from front L…R
	if (sc_boards & R1K_BOARD_MEM32_2)
		mem32_2 = make_mod_mem32("MEM2", planes, Mem32_how);
	if (sc_boards & R1K_BOARD_MEM32_0)
		mem32_0 = make_mod_mem32("MEM0", planes, Mem32_how);
	if (sc_boards & R1K_BOARD_SEQ)
		seq = make_mod_seq("SEQ", planes, Seq_how);
	if (sc_boards & R1K_BOARD_TYP)
		typ = make_mod_typ("TYP", planes, Typ_how);
	if (sc_boards & R1K_BOARD_VAL)
		val = make_mod_val("VAL", planes, Val_how);
	if (sc_boards & R1K_BOARD_FIU)
		fiu = make_mod_fiu("FIU", planes, Fiu_how);
	if (sc_boards & R1K_BOARD_IOC)
		ioc = make_mod_ioc("IOC", planes, Ioc_how);

	if (!(sc_boards & R1K_BOARD_IOC))
		planes.GB_ECC_STOP = sc_logic_0;

	planes.GB_SLOT0 = sc_logic_0;
	planes.GB_SLOT1 = sc_logic_0;
	planes.GB_CLOCK_DISABLE = sc_logic_0;

	PowerSequencer powseq("UNCLAMP");
	powseq.clamp(planes.GB_CLAMP);	// CLAMP
	powseq.reset(planes.GB_RESET);

	BackPlaneClocks bpclock("BPCLOCK");
	bpclock.bp_clk_2x_(planes.GB_BP_CLK_2X_);
	bpclock.bp_clk_2x(planes.GB_BP_CLK_2X);
	bpclock.bp_phase_2x(planes.GB_BP_PHASE2X);
	bpclock.clk_2xe(planes.GB_CLK_2XE);
	bpclock.clk_2xe_(planes.GB_CLK_2XE_);
	bpclock.clk_2x(planes.GB_CLK_2X);
	bpclock.clk_2x_(planes.GB_CLK_2X_);
	bpclock.clk_h1_phd(planes.GB_CLK_H1_PHD);
	bpclock.clk_h2_phd(planes.GB_CLK_H2_PHD);
	bpclock.clk_h1e(planes.GB_CLK_H1E);
	bpclock.clk_h2e(planes.GB_CLK_H2E);
	bpclock.clk_h1(planes.GB_CLK_H1);
	bpclock.clk_h2(planes.GB_CLK_H2);
	bpclock.clk_q1_(planes.GB_CLK_Q1_);
	bpclock.clk_q2_(planes.GB_CLK_Q2_);
	bpclock.clk_q3_(planes.GB_CLK_Q3_);
	bpclock.clk_q4_(planes.GB_CLK_Q4_);

	planes.GB_EXT_ID0 = sc_logic_0;
	planes.GB_EXT_ID1 = sc_logic_1;
	planes.GB_EXT_ID2 = sc_logic_0;

	if (0) {
		sc_trace(planes.tf, planes.GB_BP_CLK_2X_, "GB.BP_CLK_2X~");
		sc_trace(planes.tf, planes.GB_BP_CLK_2X, "GB.BP_CLK_2X");
		sc_trace(planes.tf, planes.GB_BP_PHASE2X, "GB.BP_PHASE2X");
		sc_trace(planes.tf, planes.GB_CLK_2XE_, "GB.CLK_2XE~");
		sc_trace(planes.tf, planes.GB_CLK_2XE, "GB.CLK_2XE");
		sc_trace(planes.tf, planes.GB_CLK_2X_, "GB.CLK_2X~");
		sc_trace(planes.tf, planes.GB_CLK_2X, "GB.CLK_2X");
		sc_trace(planes.tf, planes.GB_CLK_H1_PHD, "GB.CLK_H1_PHD");
		sc_trace(planes.tf, planes.GB_CLK_H2_PHD, "GB.CLK_H2_PHD");
		sc_trace(planes.tf, planes.GB_CLK_H1E, "GB.CLK_H1E");
		sc_trace(planes.tf, planes.GB_CLK_H2E, "GB.CLK_H2E");
		sc_trace(planes.tf, planes.GB_CLK_H1, "GB.CLK_H1");
		sc_trace(planes.tf, planes.GB_CLK_H2, "GB.CLK_H2");
		sc_trace(planes.tf, planes.GB_CLK_Q1_, "GB.CLK_Q1_");
		sc_trace(planes.tf, planes.GB_CLK_Q2_, "GB.CLK_Q2_");
		sc_trace(planes.tf, planes.GB_CLK_Q3_, "GB.CLK_Q3_");
		sc_trace(planes.tf, planes.GB_CLK_Q4_, "GB.CLK_Q4_");
	}

	sc_set_time_resolution(1, SC_NS);

	cout << sc_get_time_resolution() <<  " Resolution\n";

	sc_start(SC_ZERO_TIME);
	while(1) {
		double dt = sc_main_get_quota();
		sc_start(dt * 1e6, SC_US);
		cout << "@" << sc_time_stamp() << " DONE\n";
		sc_close_vcd_trace_file(planes.tf);
	}

	return(0);
}

double
sc_when(void)
{
	return (sc_time_stamp().to_seconds());
}
