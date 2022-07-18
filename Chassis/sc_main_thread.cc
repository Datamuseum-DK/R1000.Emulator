#include <stdio.h>
#include <systemc.h>

#include "Chassis/r1000sc.h"
#include "Chassis/r1000sc_priv.h"


double
sc_now(void)
{
	return (sc_time_stamp().to_double());
}

#include "planes.hh"

#include "emu_board_pub.hh"
#include "fiu_board_pub.hh"
#include "ioc_board_pub.hh"
#include "mem32_board_pub.hh"
#include "seq_board_pub.hh"
#include "typ_board_pub.hh"
#include "val_board_pub.hh"

static char Emu_how[] = "+";
static char Fiu_how[] = "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
static char Ioc_how[] = "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
static char Mem32_how[] = "+++++++++++++++++++++++++++++++++++++++++++++++";
static char Seq_how[] = "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
static char Typ_how[] = "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
static char Val_how[] = "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";

SC_MODULE(PowerSequencer)
{
	sc_out <bool> clamp;	// CLAMP
	sc_out <sc_logic> reset;	// RESET
	int do_trace = 0;

	SC_CTOR(PowerSequencer)
	{
		SC_THREAD(thread);
	}

	void thread()
	{
		clamp = false;
		reset = sc_logic_0;
		wait(100, SC_NS);
		clamp = true;
		wait(200, SC_NS);
		/*
		 * When running IOC experiments without the IOP we need
		 * to force the reset signal high because DREG4 pulls it
		 * low on IOC_RESET and the IOP will not be releasing it.
		 * (1+0 = 'X' but that is good enough for now.)
		 */
		if (sc_forced_reset)
			reset = sc_logic_1;
		else
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
	mod_emu *emu;

	(void)argc;
	(void)argv;

	mod_planes planes("PLANES");
	planes.tf = sc_create_vcd_trace_file(tracepath);

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

	emu = make_mod_emu("EMU", planes, Emu_how);

	planes.PD = false;
	planes.PU = true;

	if (!(sc_boards & R1K_BOARD_IOC))
		planes.ECC_STOP_EN = false;

	planes.B_SLOT0 = false;
	planes.B_SLOT1 = false;

	PowerSequencer powseq("UNCLAMP");
	powseq.clamp(planes.CLAMPnot);	// CLAMP
	powseq.reset(planes.RESETnot);

	planes.EXT_ID0 = false;
	planes.EXT_ID1 = true;
	planes.EXT_ID2 = false;

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
