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
   1MEG DRAM
   =========

'''

from part import PartModel, PartFactory

class DRAM1MEG(PartFactory):

    ''' 1MEG DRAM '''

    def state(self, file):
        file.fmt('''
		|	unsigned ras, cas;
		|	uint32_t bits[(1<<20)>>5];
		|''')

    def sensitive(self):
        yield "PIN_RAS.neg()"
        yield "PIN_CAS"
        yield "PIN_WE"

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	uint32_t adr = 0, data, mask;
		|
		|	BUS_A_READ(adr);
		|
		|	if (PIN_RAS.negedge())
		|		state->ras = adr;
		|	if (PIN_CAS.negedge()) {
		|		state->cas = adr;
		|		adr = (state->cas << 10) | state->ras;
		|		mask = adr & 0x1f;
		|		adr >>= 5;
		|		if (!PIN_WE=>) {
		|			PIN_DQ = sc_logic_Z;
		|			if (PIN_DQ=>)
		|				state->bits[adr] |= mask;
		|			else
		|				state->bits[adr] &= ~mask;
		|		} else {
		|			data = state->bits[adr] & mask;
		|			PIN_DQ = AS(data);
		|		}
		|	}
		|	if (PIN_RAS.posedge() || PIN_CAS.posedge()) {
		|		PIN_DQ = sc_logic_Z;
		|	}
		|	TRACE(
		|	    << " ras " << PIN_RAS?
		|	    << " cas " << PIN_CAS?
		|	    << " we " << PIN_WE?
		|	    << " a " << BUS_A_TRACE()
		|	    << " dq " << PIN_DQ?
		|	);
		|
		|''')

class Model1Meg(PartModel):
    ''' 1MEG DRAM '''


    def assign(self, comp):

        node = comp["D"]
        node.remove()
        node = comp["Q"]
        node.remove()
        node.pin.name = "DQ"
        node.pin.role = "sc_inout_resolved"
        node.insert()
        super().assign(comp)

def register(board):
    ''' Register component model '''

    board.add_part("1MEG", Model1Meg("1MEG", DRAM1MEG))
