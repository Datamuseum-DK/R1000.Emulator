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

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	uint32_t tmp, tmp2, out = 0;
		|
		|	BUS_S_READ(tmp);
		|	tmp = (tmp ^ (tmp >> 2)) & 0x03;
		|	tmp = (tmp ^ (tmp >> 1)) & 0x01;
		|	if (tmp)
		|		out |= 0x100;
		|
		|	BUS_N_READ(tmp);
		|	tmp = (tmp ^ (tmp >> 4)) & 0x0f0f0f0f;
		|	tmp = (tmp ^ (tmp >> 2)) & 0x03030303;
		|	tmp = (tmp ^ (tmp >> 1)) & 0x01010101;
		|	if (tmp & 0x01000000)
		|		out |= 0x80;
		|	if (tmp & 0x010000)
		|		out |= 0x40;
		|	if (tmp & 0x0100)
		|		out |= 0x20;
		|	if (tmp & 0x01)
		|		out |= 0x10;
		|
		|	BUS_O_READ(tmp2);
		|
		|	tmp = tmp2 >> 17;
		|	tmp = (tmp ^ (tmp >> 4)) & 0x0f;
		|	tmp = (tmp ^ (tmp >> 2)) & 0x03;
		|	tmp = (tmp ^ (tmp >> 1)) & 0x01;
		|	if (tmp & 0x01)
		|		out |= 0x8;
		|
		|	tmp = (tmp2 >> 6) & 0x7ff;
		|	tmp = (tmp ^ (tmp >> 8)) & 0xff;
		|	tmp = (tmp ^ (tmp >> 4)) & 0x0f;
		|	tmp = (tmp ^ (tmp >> 2)) & 0x03;
		|	tmp = (tmp ^ (tmp >> 1)) & 0x01;
		|	if (tmp & 0x01)
		|		out |= 0x4;
		|
		|	tmp = tmp2 & 0x3f;
		|	tmp = (tmp ^ (tmp >> 4)) & 0x0f;
		|	tmp = (tmp ^ (tmp >> 2)) & 0x03;
		|	tmp = (tmp ^ (tmp >> 1)) & 0x01;
		|	if (tmp & 0x01)
		|		out |= 0x2;
		|
		|	BUS_B_READ(tmp);
		|	tmp = (tmp ^ (tmp >> 4)) & 0x0f;
		|	tmp = (tmp ^ (tmp >> 2)) & 0x03;
		|	tmp = (tmp ^ (tmp >> 1)) & 0x01;
		|	if (tmp & 0x01)
		|		out |= 0x1;
		|
		|	if (PIN_TST=>)
		|		out ^= BUS_PEV_MASK;
		|
		|	TRACE(
		|	    << " s " << BUS_S_TRACE()
		|	    << " n " << BUS_N_TRACE()
		|	    << " o " << BUS_O_TRACE()
		|	    << " b " << BUS_B_TRACE()
		|	    << " tst " << PIN_TST?
		|	    << " out " << std::hex << out
		|	);
		|
		|	BUS_POD_WRITE(out);
		|	out ^= BUS_PEV_MASK;
		|	BUS_PEV_WRITE(out);
		|
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("XADRPAR", PartModel("XADRPAR", XADRPAR))
