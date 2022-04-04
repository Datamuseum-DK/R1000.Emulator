#!/usr/local/bin/python3
#
# Copyright (c) 2021 Poul-Henning Kamp
# All rights reserved.
#
# Author: Poul-Henning Kamp <phk@phk.freebsd.dk>
#
# SPDX-License-Identifier: BSD-2-Clause
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

'''
   128x4 NOVRAM
   ============

'''

from part import PartModel, PartFactory

class NOVRAM(PartFactory):

    ''' 128x4 NOVRAM '''

    def state(self, file):
        ''' Extra state variable '''

        file.write("\tuint8_t ram[256];\n")

    def init(self, file):
        ''' Extra initialization '''

        file.fmt('''
		|	memset(state->ram, 0, sizeof state->ram);
		|	int i, j;
		|	should_i_trace(this->name(), &state->ctx.do_trace);
		|
		|	if (strstr(arg, "FIU")) {
		|		load_programmable(arg, state->ram, sizeof state->ram,
		|		    "R1000_FIU_NOVRAM");
		|	} else if (strstr(arg, "VAL")) {
		|		load_programmable(arg, state->ram, sizeof state->ram,
		|		    "R1000_VAL_NOVRAM");
		|	} else if (strstr(arg, "TYP")) {
		|		load_programmable(arg, state->ram, sizeof state->ram,
		|		    "R1000_TYP_NOVRAM");
		|	} else if (strstr(arg, "SEQ")) {
		|		load_programmable(arg, state->ram, sizeof state->ram,
		|		    "R1000_SEQ_NOVRAM");
		|	} else if (strstr(arg, "MEM0")) {
		|		load_programmable(arg, state->ram, sizeof state->ram,
		|		    "R1000_MEM0_NOVRAM");
		|	} else if (strstr(arg, "MEM2")) {
		|		load_programmable(arg, state->ram, sizeof state->ram,
		|		    "R1000_MEM2_NOVRAM");
		|	} else {
		|		unsigned part, serial, artwork, eco, year, month, day, tstamp;
		|
		|		part = 99; serial = 9999; artwork = 99; eco = 99;
		|		year = 1998; month = 9; day = 9;
		|
		|		state->ram[0x01] = (part >> 4) & 0xf;
		|		state->ram[0x02] = part & 0xf;
		|
		|		state->ram[0x03] = (serial >> 12) & 0xf;
		|		state->ram[0x04] = (serial >> 8) & 0xf;
		|		state->ram[0x05] = (serial >> 4) & 0xf;
		|		state->ram[0x06] = serial & 0xf;
		|
		|		state->ram[0x07] = (artwork >> 4) & 0xf;
		|		state->ram[0x08] = artwork & 0xf;
		|
		|		state->ram[0x09] = (eco >> 4) & 0xf;
		|		state->ram[0x0a] = eco & 0xf;
		|
		|		tstamp = (year - 1901) << 9;
		|		tstamp |= month << 5;
		|		tstamp |= day;
		|
		|		state->ram[0x0b] = (tstamp >> 12) & 0xf;
		|		state->ram[0x0c] = (tstamp >> 8) & 0xf;
		|		state->ram[0x0d] = (tstamp >> 4) & 0xf;
		|		state->ram[0x0e] = tstamp & 0xf;
		|
		|		j = 0x56;
		|		for (i = 1; i < 0x0f; i += 2) {
		|			j += state->ram[i] << 4;
		|			j += state->ram[i + 1];
		|		}
		|		j = ~j;
		|		j += 1;
		|		j &= 0xff;
		|		printf("NOVRAM CHECKSUM1 0x%x\\n", j);
		|		state->ram[0x0f] = (j >> 4) & 0xf;
		|		state->ram[0x10] = j & 0xf;
		|	}
		|
		|	// READ_NOVRAM.FIU expects zero byte XOR checksum.
		|	// We dont know which location is for adjustment, use last two.
		|	j = 0xaa;
		|	for (i = 0; i < 254; i += 2)
		|		j ^= ((state->ram[i] << 4) | state->ram[i + 1]);
		|	j &= 0xff;
		|	state->ram[0xfe] = (j >> 4 & 0xf);
		|	state->ram[0xff] = j & 0xf;
		|''')

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|
		|	unsigned adr = 0, data = 0;
		|
		|	BUS_A_READ(adr);
		|	data = state->ram[adr];
		|
		|	if (PIN_CS=>) {
		|		BUS_DQ_Z();
		|	} else if (!PIN_WE=> && PIN_STORE=> && PIN_RECALL=>) {
		|		BUS_DQ_Z();
		|		BUS_DQ_READ(data);
		|		state->ram[adr] = data;
		|	} else {
		|		BUS_DQ_WRITE(data);
		|	}
		|	TRACE(
		|	    << " cs_ " <<PIN_CS?
		|	    << " store_" <<PIN_STORE?
		|	    << " recall_ " <<PIN_RECALL?
		|	    << " we_ " <<PIN_WE?
		|	    << " a " << BUS_A_TRACE()
		|	    << " dq " <<BUS_DQ_TRACE()
		|	    << " A " << std::hex << adr
		|	    << " D " << std::hex << data
		|	);
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("NOVRAM", PartModel("NOVRAM", NOVRAM))
