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
   8x8 parity checker
   ==================

'''


from part import PartModel, PartFactory

class XPAR64(PartFactory):

    ''' 8x8 parity checker '''

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	uint64_t tmp, total = 0;
		|
		|	BUS_I_READ(tmp);
		|	tmp ^= (tmp >> 4) &  0x0f0f0f0f0f0f0f0f;
		|	tmp ^= (tmp >> 2) &  0x0303030303030303;
		|	tmp ^= (tmp >> 1) &  0x0101010101010101;
		|
		|	if (!PIN_ODD=>)
		|		tmp ^= 0x0101010101010101;
		|	tmp &= 0x0101010101010101;

		|	PIN_P0<= (tmp >> 56) & 1;
		|	PIN_P1<= (tmp >> 48) & 1;
		|	PIN_P2<= (tmp >> 40) & 1;
		|	PIN_P3<= (tmp >> 32) & 1;
		|	PIN_P4<= (tmp >> 24) & 1;
		|	PIN_P5<= (tmp >> 16) & 1;
		|	PIN_P6<= (tmp >>  8) & 1;
		|	PIN_P7<= (tmp >>  0) & 1;
		|
		|	total = tmp ^ (tmp >> 32);
		|	total ^= (total >> 16);
		|	total ^= (total >> 8);
		|	total &= 1;
		|
		|	TRACE(
		|	    << " i " << BUS_I_TRACE()
		|	    << " p " << std::hex << tmp
		|	    << " a " << total
		|	);
		|
		|	if (!PIN_ODD=>)
		|		total ^= 0x1;
		|	PIN_PALL<= total & 1;
		|
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("XPAR64", PartModel("XPAR64", XPAR64))
