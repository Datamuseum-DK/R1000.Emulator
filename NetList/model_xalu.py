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
   Zero bit counter
   ================

'''


from part import PartModel, PartFactory

class XALU(PartFactory):

    ''' Zero bit counter'''

    def extra(self, file):
        file.include("Components/tables.h")
        super().extra(file)

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	unsigned idx, a, b, s, tour, ci, y = 0, eq = 1;
		|
		|	ci = PIN_CI=>;
		|	BUS_A_READ(a);
		|	BUS_B_READ(b);
		|	BUS_S_READ(s);
		|
		|	for (tour = 0; tour < BUS_A_WIDTH; tour += 4) {
		|		idx = 0;
		|		if (ci) idx |= 1 << 13;
		|		if (PIN_M=>) idx |= 1 << 12;
		|		idx |= 0xf00 & ((a >> tour) << 8);
		|		idx |= 0x0f0 & ((b >> tour) << 4);
		|		idx |= s;
		|		unsigned val = lut181[idx];
		|		y |= ((val >> 4) & 0xf) << tour;
		|		if (!(val & 0x08))
		|			eq = 0;
		|		ci = (val & 0x02);
		|	}
		|
		|	TRACE(
		|	    << " s " << BUS_S_TRACE()
		|	    << " m " << PIN_M?
		|	    << " ci " << PIN_CI?
		|	    << " a " << BUS_A_TRACE()
		|	    << " b " << BUS_B_TRACE()
		|	    << " = " << eq
		|	    << " co " << ci
		|	);
		|	PIN_AeqB<=(eq);
		|	PIN_CO<=(ci);
		|	BUS_Y_WRITE(y);
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("XALU8", PartModel("XALU8", XALU))
