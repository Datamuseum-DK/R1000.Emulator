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
		|	uint64_t tmp, total = 0, par = 0;
		|
		|	BUS_I_READ(tmp);
		|	tmp = (tmp ^ (tmp >> 4)) & 0x0f0f0f0f0f0f0f0f;
		|	tmp = (tmp ^ (tmp >> 2)) & 0x0303030303030303;
		|	tmp = (tmp ^ (tmp >> 1)) & 0x0101010101010101;
		|
		|	if (PIN_ODD=>)
		|		tmp ^= 0x0101010101010101;
		|
		|	if (tmp & (1ULL<<56)) par |= 0x80;
		|	if (tmp & (1ULL<<48)) par |= 0x40;
		|	if (tmp & (1ULL<<40)) par |= 0x20;
		|	if (tmp & (1ULL<<32)) par |= 0x10;
		|	if (tmp & (1ULL<<24)) par |= 0x8;
		|	if (tmp & (1ULL<<16)) par |= 0x4;
		|	if (tmp & (1ULL<<8)) par |= 0x2;
		|	if (tmp & (1ULL<<0)) par |= 0x1;
		|	BUS_P_WRITE(par);
		|
		|	total = (tmp ^ (tmp >> 32)) & 0x01010101;
		|	total = (total ^ (total >> 16)) & 0x0101;
		|	total = (total ^ (total >> 8)) & 0x01;
		|
		|	if (PIN_ODD=>)
		|		total ^= 0x1;
		|
		|	TRACE(
		|	    << " i " << BUS_I_TRACE()
		|	    << " p " << std::hex << par
		|	    << " a " << total
		|	);
		|
		|	PIN_PALL<= total & 1;
		|
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("XPAR64", PartModel("XPAR64", XPAR64))
