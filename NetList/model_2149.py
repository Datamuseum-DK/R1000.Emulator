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
   2149 CMOS Static RAM 1024 x 4 bit
   =================================

'''


from part import PartModel, PartFactory

class SRAM2149(PartFactory):

    ''' 2149 CMOS Static RAM 1024 x 4 bit '''

    def state(self, file):
        file.fmt('''
		|	uint8_t ram[1024];
		|''')

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	unsigned adr = 0, wrote = 0;
		|
		|
		|''')

        if not self.comp.nodes["CS"].net.is_pd():
            file.fmt('''
		|	if (PIN_CS=>) {
		|		// TRACE(<< "z");
		|		PIN_DQ0 = sc_logic_Z;
		|		PIN_DQ1 = sc_logic_Z;
		|		PIN_DQ2 = sc_logic_Z;
		|		PIN_DQ3 = sc_logic_Z;
		|		next_trigger(PIN_CS.negedge_event());
		|		return;
		|	}
		|''')

        file.fmt('''
		|
		|	if (PIN_A9=>) adr |= 1 << 0;
		|	if (PIN_A8=>) adr |= 1 << 1;
		|	if (PIN_A7=>) adr |= 1 << 2;
		|	if (PIN_A6=>) adr |= 1 << 3;
		|	if (PIN_A5=>) adr |= 1 << 4;
		|	if (PIN_A4=>) adr |= 1 << 5;
		|	if (PIN_A3=>) adr |= 1 << 6;
		|	if (PIN_A2=>) adr |= 1 << 7;
		|	if (PIN_A1=>) adr |= 1 << 8;
		|	if (PIN_A0=>) adr |= 1 << 9;
		|
		|	if (!PIN_WE=>) {
		|		PIN_DQ0 = sc_logic_Z;
		|		PIN_DQ1 = sc_logic_Z;
		|		PIN_DQ2 = sc_logic_Z;
		|		PIN_DQ3 = sc_logic_Z;
		|		state->ram[adr] = 0;
		|		if (PIN_DQ0=>) state->ram[adr] |= (1<<3);
		|		if (PIN_DQ1=>) state->ram[adr] |= (1<<2);
		|		if (PIN_DQ2=>) state->ram[adr] |= (1<<1);
		|		if (PIN_DQ3=>) state->ram[adr] |= (1<<0);
		|		wrote = 1;
		|	}
		|	if (wrote || (state->ctx.do_trace & 2)) {
		|		TRACE(
		|		    << " cs " << PIN_CS?
		|		    << " we " << PIN_WE?
		|		    << " a " << PIN_A0? << PIN_A1? << PIN_A2?
		|		    << PIN_A3? << PIN_A4? << PIN_A5? << PIN_A6?
		|		    << PIN_A7? << PIN_A8? << PIN_A9?
		|		    << " dq "
		|		    << PIN_DQ0?
		|		    << PIN_DQ1?
		|		    << PIN_DQ2?
		|		    << PIN_DQ3?
		|		    << " | "
		|		    << std::hex << adr
		|		    << " "
		|		    << std::hex << (unsigned)(state->ram[adr])
		|		);
		|	}
		|	if (PIN_WE=>) {
		|		PIN_DQ0<=(state->ram[adr] & (1<<3));
		|		PIN_DQ1<=(state->ram[adr] & (1<<2));
		|		PIN_DQ2<=(state->ram[adr] & (1<<1));
		|		PIN_DQ3<=(state->ram[adr] & (1<<0));
		|	}
		|''')

class Model2149(PartModel):

    def assign(self, comp):
        if comp.nodes["CS"].net.is_pd():
            for node in comp:
                if node.pin.name[:2] == "DQ":
                    node.pin.role = "c_output"
        super().assign(comp)


def register(board):
    ''' Register component model '''

    board.add_part("2149", Model2149("2149", SRAM2149))
