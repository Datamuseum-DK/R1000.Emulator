#ifndef R1000_OSC
#define R1000_OSC

// #include "r1000sc.h"

extern int64_t simclock;

struct scm_osc_state;

SC_MODULE(SCM_OSC)
{
	sc_out <sc_logic> pin8;
	unsigned freq;
	long half_period;

	SC_HAS_PROCESS(SCM_OSC);

	SCM_OSC(sc_module_name nm, const char *arg) :
		sc_module(nm)
	{
		freq = strtoul(arg, NULL, 0);
		if (false and freq) {
			half_period = (1000000000 / freq) / 2;
			SC_THREAD(osc_main);
			cout
			    << "OSC " << this->name()
			    << " freq " << freq
			    << " half-period " << half_period
			    << " ns \n";
		}
	}

	void osc_main()
	{
		while(1) {
			pin8 = sc_logic_1;
			wait(half_period, SC_NS);
			simclock += half_period;
			pin8 = sc_logic_0;
			wait(half_period, SC_NS);
			simclock += half_period;
		}
	}
};

#endif /* R1000_OSC */
