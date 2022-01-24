
#include <systemc.h>
#include "context.h"

#include "r1000sc.h"
#include "r1000sc_priv.h"
#include "NOVRAM.hh"

struct scm_novram_state {
	struct ctx ctx;
	uint8_t ram[256];
};

void
SCM_NOVRAM :: loadit(const char *arg)
{

	state = (struct scm_novram_state *)
	    CTX_Get("novram", this->name(), sizeof *state);
	memset(state->ram, 0, sizeof state->ram);
	int i, j;
	should_i_trace(this->name(), &state->ctx.do_trace);

	if (strstr(arg, "FIU")) {
		load_programmable(arg, state->ram, sizeof state->ram,
		    "R1000_FIU_NOVRAM");
	} else if (strstr(arg, "VAL")) {
		load_programmable(arg, state->ram, sizeof state->ram,
		    "R1000_VAL_NOVRAM");
	} else if (strstr(arg, "TYP")) {
		load_programmable(arg, state->ram, sizeof state->ram,
		    "R1000_TYP_NOVRAM");
	} else if (strstr(arg, "SEQ")) {
		load_programmable(arg, state->ram, sizeof state->ram,
		    "R1000_SEQ_NOVRAM");
	} else if (strstr(arg, "MEM0")) {
		load_programmable(arg, state->ram, sizeof state->ram,
		    "R1000_MEM0_NOVRAM");
	} else if (strstr(arg, "MEM2")) {
		load_programmable(arg, state->ram, sizeof state->ram,
		    "R1000_MEM2_NOVRAM");
	} else {
		unsigned part, serial, artwork, eco, year, month, day, tstamp;

		part = 99; serial = 9999; artwork = 99; eco = 99;
		year = 1998; month = 9; day = 9;

		state->ram[0x01] = (part >> 4) & 0xf;
		state->ram[0x02] = part & 0xf;

		state->ram[0x03] = (serial >> 12) & 0xf;
		state->ram[0x04] = (serial >> 8) & 0xf;
		state->ram[0x05] = (serial >> 4) & 0xf;
		state->ram[0x06] = serial & 0xf;

		state->ram[0x07] = (artwork >> 4) & 0xf;
		state->ram[0x08] = artwork & 0xf;

		state->ram[0x09] = (eco >> 4) & 0xf;
		state->ram[0x0a] = eco & 0xf;

		tstamp = (year - 1901) << 9;
		tstamp |= month << 5;
		tstamp |= day;

		state->ram[0x0b] = (tstamp >> 12) & 0xf;
		state->ram[0x0c] = (tstamp >> 8) & 0xf;
		state->ram[0x0d] = (tstamp >> 4) & 0xf;
		state->ram[0x0e] = tstamp & 0xf;

		j = 0x56;
		for (i = 1; i < 0x0f; i += 2) {
			j += state->ram[i] << 4;
			j += state->ram[i + 1];
		}
		j = ~j;
		j += 1;
		j &= 0xff;
		printf("NOVRAM CHECKSUM1 0x%x\n", j);
		state->ram[0x0f] = (j >> 4) & 0xf;
		state->ram[0x10] = j & 0xf;
	}

	// READ_NOVRAM.FIU expects zero byte XOR checksum.
	// We dont know which location is for adjustment, use last two.
	j = 0xaa;
	for (i = 0; i < 254; i += 2)
		j ^= ((state->ram[i] << 4) | state->ram[i + 1]);
	j &= 0xff;
	state->ram[0xfe] = (j >> 4 & 0xf);
	state->ram[0xff] = j & 0xf;
}


void
SCM_NOVRAM :: doit(void)
{
	unsigned adr = 0, data = 0;

	state->ctx.activations++;
	if (IS_H(pin1)) adr |= (1<<0);
	if (IS_H(pin17)) adr |= (1<<1);
	if (IS_H(pin16)) adr |= (1<<2);
	if (IS_H(pin8)) adr |= (1<<3);
	if (IS_H(pin3)) adr |= (1<<4);
	if (IS_H(pin4)) adr |= (1<<5);
	if (IS_H(pin5)) adr |= (1<<6);
	if (IS_H(pin6)) adr |= (1<<7);
	data = state->ram[adr];

	if (IS_H(pin7)) {
		pin15 = sc_logic_Z;
		pin14 = sc_logic_Z;
		pin13 = sc_logic_Z;
		pin12 = sc_logic_Z;
	} else if (IS_L(pin11) && IS_H(pin9) && IS_H(pin10)) {
		pin15 = sc_logic_Z;
		pin14 = sc_logic_Z;
		pin13 = sc_logic_Z;
		pin12 = sc_logic_Z;
		data = 0;
		if (IS_H(pin15)) data |= 8;
		if (IS_H(pin14)) data |= 4;
		if (IS_H(pin13)) data |= 2;
		if (IS_H(pin12)) data |= 1;
		state->ram[adr] = data;
	} else {
		pin15 = AS(data & 8);
		pin14 = AS(data & 4);
		pin13 = AS(data & 2);
		pin12 = AS(data & 1);
	}
	TRACE(
	    << " cs_ " <<pin7
	    << " store_" <<pin9
	    << " recall_ " <<pin10
	    << " we_ " <<pin11
	    << " a " << pin6 << pin5 << pin4 << pin3 << pin8 << pin16
		<< pin17 << pin1
	    << " dq " <<pin15 <<pin14 <<pin13 <<pin12
	    << " A " << std::hex << adr
	    << " D " << std::hex << data
	);
}
