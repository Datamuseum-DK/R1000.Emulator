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
   NXM SRAM
   ==========

'''


from part import PartModel, PartFactory

class XRFRAM(PartFactory):

    ''' NXM SRAM '''

    def state(self, file):
        file.fmt('''
		|	uint64_t ram[1<<BUS_A_WIDTH];
		|	uint64_t last;
		|	const char *what;
		|''')

    def extra(self, file):
        super().extra(file)
        file.fmt('''
		|static const char *READING = "r";
		|static const char *WRITING = "w";
		|static const char *ZZZING = "z";
		|''')

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	unsigned adr = 0;
		|	uint64_t data = 0;
		|
		|	BUS_A_READ(adr);
		|
		|	if (PIN_CS=>) {
		|		if (state->what == READING) {
		|			BUS_DQ_Z();
		|		} else if (state->what == WRITING) {
		|			BUS_DQ_READ(data);
		|			state->ram[adr] = data;
		|		}
		|		next_trigger(PIN_CS.negedge_event());
		|		state->what = ZZZING;
		|	} else if (!PIN_WE=>) {
		|		if (state->what == READING)
		|			BUS_DQ_Z();
		|		BUS_DQ_READ(data);
		|		state->ram[adr] = data;
		|		state->what = WRITING;
		|	} else {
		|		if (state->what == WRITING) {
		|			BUS_DQ_READ(data);
		|			state->ram[adr] = data;
		|		}
		|		data = state->ram[adr];
		|		if (state->what != READING || data != state->last) {
		|			BUS_DQ_WRITE(data);
		|			state->last = data;
		|		}
		|		state->what = READING;
		|	}
		|
		|	TRACE(
		|	    << state->what
		|	    << " we " << PIN_WE?
		|	    << " cs " << PIN_CS?
		|	    << " a " << BUS_A_TRACE()
		|	    << " d " << BUS_DQ_TRACE()
		|	);
		|''')

class ThisRam(PartModel):
    ''' ... '''

    def assign(self, comp):
        for node in comp:
            if node.pin.name[:2] == "IO":
                node.pin.name = "DQ" + node.pin.name[2:]
                node.pin.update()
        super().assign(comp)

def register(board):
    ''' Register component model '''

    board.add_part("XRFRAM", PartModel("XRFRAM", XRFRAM))
    board.add_part("XTAGRAM", ThisRam("XTAGRAM", XRFRAM))
    board.add_part("16KX4", ThisRam("16KX4", XRFRAM))
    board.add_part("16KX8", ThisRam("16KX8", XRFRAM))
