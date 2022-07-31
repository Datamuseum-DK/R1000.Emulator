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
   Address bus parity checker
   ==========================

'''


from part import PartModel, PartFactory

class XADRPAR(PartFactory):

    ''' Address bus parity checker '''

    def state(self, file):
        file.fmt('''
		|       unsigned par;
		|''')

    def sensitive(self):
        yield ("PIN_CLK.pos()")
        yield ("PIN_OE")

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	uint64_t a, tmp;
		|
		|	if (PIN_CLK.posedge()) {
		|		state->par = 0;
		|		BUS_A_READ(a);
		|		tmp = a;
		|		tmp = (tmp ^ (tmp >> 4)) & 0x0f0f0f0f0f000000;
		|		tmp = (tmp ^ (tmp >> 2)) & 0x0303030303000000;
		|		tmp = (tmp ^ (tmp >> 1)) & 0x0101010101000000;
		|
		|		if (tmp & (1ULL<<56)) state->par |= 0x80;
		|		if (tmp & (1ULL<<48)) state->par |= 0x40;
		|		if (tmp & (1ULL<<40)) state->par |= 0x20;
		|		if (tmp & (1ULL<<32)) state->par |= 0x10;
		|		if (tmp & (1ULL<<24)) state->par |= 0x8;
		|
		|		tmp = (a & 0xffe000) >> 13;
		|		tmp = (tmp ^ (tmp >> 8)) & 0xff;
		|		tmp = (tmp ^ (tmp >> 4)) & 0x0f;
		|		tmp = (tmp ^ (tmp >> 2)) & 0x03;
		|		tmp = (tmp ^ (tmp >> 1)) & 0x01;
		|		if (tmp & 1) state->par |= 0x4;
		|
		|		tmp = (a & 0x1f80) >> 7;
		|		tmp = (tmp ^ (tmp >> 4)) & 0x0f;
		|		tmp = (tmp ^ (tmp >> 2)) & 0x03;
		|		tmp = (tmp ^ (tmp >> 1)) & 0x01;
		|		if (tmp & 1) state->par |= 0x2;
		|
		|		tmp = a & 0x7f;
		|		tmp = (tmp ^ (tmp >> 4)) & 0x0f;
		|		tmp = (tmp ^ (tmp >> 2)) & 0x03;
		|		tmp = (tmp ^ (tmp >> 1)) & 0x01;
		|		if (tmp & 1) state->par |= 0x1;
		|
		|		if (PIN_ODD=>)
		|			state->par ^= 0xff;
		|	}
		|	if (PIN_OE=>) {
		|		BUS_AP_WRITE(state->par);
		|	} else {
		|		BUS_AP_Z();
		|	}
		|
		|	TRACE(
		|	    << " a " << BUS_A_TRACE()
		|	    << " clk " << PIN_CLK.posedge()
		|	    << " oe " << PIN_OE
		|	    << " ap " << std::hex << state->par
		|	);
		|
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("XADRPAR", PartModel("XADRPAR", XADRPAR))
