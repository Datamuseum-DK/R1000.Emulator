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
   2167 CMOS Static RAM 16K x 1-Bit
   ================================

   Ref: Rensas DSC2981/08 February 2001
'''


from part import PartModel, PartFactory

class SRAM2167(PartFactory):

    ''' 2167 CMOS Static RAM 16K x 1-Bit '''

    def state(self, file):
        file.fmt('''
		|	bool ram[16384];
		|''')

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	unsigned adr = 0;
		|
		|	state->ctx.activations++;
		|	if (PIN_A0=>) adr |= 1 << 13;
		|	if (PIN_A1=>) adr |= 1 << 12;
		|	if (PIN_A2=>) adr |= 1 << 11;
		|	if (PIN_A3=>) adr |= 1 << 10;
		|	if (PIN_A4=>) adr |= 1 << 9;
		|	if (PIN_A5=>) adr |= 1 << 8;
		|	if (PIN_A6=>) adr |= 1 << 7;
		|	if (PIN_A7=>) adr |= 1 << 6;
		|	if (PIN_A8=>) adr |= 1 << 5;
		|	if (PIN_A9=>) adr |= 1 << 4;
		|	if (PIN_A10=>) adr |= 1 << 3;
		|	if (PIN_A11=>) adr |= 1 << 2;
		|	if (PIN_A12=>) adr |= 1 << 1;
		|	if (PIN_A13=>) adr |= 1 << 0;
		|''')

        if self.comp.nodes["CS"].net.is_pd():
            file.fmt('''
		|	if (!PIN_WE=>)
		|		state->ram[adr] = PIN_D=>;
		|	PIN_Q<=(state->ram[adr]);
		|''')
        else:
            file.fmt('''
		|	if (PIN_CS=>) {
		|		PIN_Q = sc_logic_Z;
		|	} else {
		|		if (!PIN_WE=>)
		|			state->ram[adr] = PIN_D=>;
		|		PIN_Q<=(state->ram[adr]);
		|	}
		|''')

        file.fmt('''
		|	TRACE(
		|	    << " a "
		|	    << PIN_A0?
		|	    << PIN_A1?
		|	    << PIN_A2?
		|	    << PIN_A3?
		|	    << PIN_A4?
		|	    << PIN_A5?
		|	    << PIN_A6?
		|	    << PIN_A7?
		|	    << PIN_A8?
		|	    << PIN_A9?
		|	    << PIN_A10?
		|	    << PIN_A11?
		|	    << PIN_A12?
		|	    << PIN_A13?
		|	    << " d "
		|	    << PIN_D?
		|	    << " w "
		|	    << PIN_WE?
		|	    << " cs "
		|	    << PIN_CS?
		|	    << " | "
		|	    << std::hex << adr
		|	    << " "
		|	    << state->ram[adr]
		|	);
		|''')

class Model_2167(PartModel):

    ''' Fix Q pin to be tri-state '''

    def assign(self, comp):
        if comp.nodes["CS"].net.is_pd():
            comp.nodes["Q"].pin.role = "c_output"
        super().assign(comp)


def register(board):
    ''' Register component model '''

    board.add_part("2167", Model_2167("2167", SRAM2167))
