#include <stdio.h>
#include <systemc.h>

#include "r1000sc.h"
#include "r1000sc_priv.h"


double
sc_now(void)
{
	return (sc_simulation_time());
}

int debug_flag = 0;

#include "../Planes/planes.hh"

#include "../Ioc/main/ioc.pub.hh"
#include "../Seq/main/seq.pub.hh"
#include "../Fiu/main/fiu.pub.hh"
#include "../Typ/main/typ.pub.hh"
#include "../Val/main/val.pub.hh"
#include "../Mem32/main/mem32.pub.hh"

SC_MODULE(BackPlaneClocks)
{
	sc_out <sc_logic> gb288;	// BP.CLK.2X~
	sc_out <sc_logic> gb294;	// BP.CLK.2X
	sc_out <sc_logic> gb295;	// BP.PHASE.2X
	sc_out <sc_logic> gb296;	// CLOCK_DISABLE~

	SC_CTOR(BackPlaneClocks)
	{
		SC_THREAD(thread);
	}

	void thread()
	{
		gb296 = sc_logic_0;
		while (1) {
			gb294 = sc_logic_1; gb288 = sc_logic_0;
			gb295 = sc_logic_0;
			wait(50, SC_NS);
			gb294 = sc_logic_0; gb288 = sc_logic_1;
			wait(50, SC_NS);
			gb294 = sc_logic_1; gb288 = sc_logic_0;
			gb295 = sc_logic_1;
			wait(50, SC_NS);
			gb294 = sc_logic_0; gb288 = sc_logic_1;
			wait(50, SC_NS);
		}
	}
};


SC_MODULE(PowerSequencer)
{
	sc_out <sc_logic> gb313;	// CLAMP
	sc_out <sc_logic> gb244;	// RESET
	int do_trace = 0;

	SC_CTOR(PowerSequencer)
	{
		SC_THREAD(thread);
	}

	void thread()
	{
		gb313 = sc_logic_0;
		gb244 = sc_logic_0;
		wait(100, SC_NS);
		gb313 = sc_logic_1;
		wait(200, SC_NS);
		gb244 = sc_logic_1;
	}
};

extern "C"
void *
sc_main_thread(void *priv)
{
	int argc = 0;
	char **argv = NULL;
	int retval;

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

	mod_planes planes("PLANES");
	planes.tf = sc_create_vcd_trace_file("/critter/_r1000");

	// Order as seen from front L…R
	if (sc_boards & R1K_BOARD_MEM32_2)
		mem32_2 = make_mod_mem32("MEM2", planes, NULL);
	if (sc_boards & R1K_BOARD_MEM32_0)
		mem32_0 = make_mod_mem32("MEM0", planes, NULL);
	if (sc_boards & R1K_BOARD_SEQ)
		seq = make_mod_seq("SEQ", planes, NULL);
	if (sc_boards & R1K_BOARD_TYP)
		typ = make_mod_typ("TYP", planes, NULL);
	if (sc_boards & R1K_BOARD_VAL)
		val = make_mod_val("VAL", planes, NULL);
	if (sc_boards & R1K_BOARD_FIU)
		fiu = make_mod_fiu("FIU", planes, NULL);
	if (sc_boards & R1K_BOARD_IOC)
		ioc = make_mod_ioc("IOC", planes, NULL);

	planes.GB319 = sc_logic_0;	// B_SLOT0
	planes.GB320 = sc_logic_0;	// B_SLOT0

	PowerSequencer powseq("UNCLAMP");
	powseq.gb313(planes.GB313);
	powseq.gb244(planes.GB244);

	BackPlaneClocks bpclock("BPCLOCK");
	bpclock.gb288(planes.GB288);
	bpclock.gb294(planes.GB294);
	bpclock.gb295(planes.GB295);
	bpclock.gb296(planes.GB296);

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
