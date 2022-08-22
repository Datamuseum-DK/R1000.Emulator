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
   MEM32 HASH generator
   ====================

'''

from part import PartModel, PartFactory

class XHASH(PartFactory):

    ''' MEM32 HASH generator '''

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)
        return

        file.fmt('''
		|	uint32_t s, n, o, hash = 0;
		|
		|	BUS_S_READ(s);
		|	BUS_N_READ(n);
		|	BUS_O_READ(o);
		|#define GBIT(fld,bit,width) ((fld >> (width - bit -1)) & 1)
		|	if (GBIT(s, 1, BUS_S_WIDTH) ^
		|	    GBIT(n, 12, BUS_N_WIDTH) ^
		|	    GBIT(o, 17, BUS_O_WIDTH))
		|		hash |= 1<<11;
		|	if (GBIT(o, 8, BUS_O_WIDTH) ^ GBIT(n, 13, BUS_N_WIDTH))
		|		hash |= 1<<10;
		|	if (GBIT(o, 9, BUS_O_WIDTH) ^ GBIT(n, 14, BUS_N_WIDTH))
		|		hash |= 1<<9;
		|	if (GBIT(o, 10, BUS_O_WIDTH) ^ GBIT(n, 15, BUS_N_WIDTH))
		|		hash |= 1<<8;
		|	if (GBIT(o, 7, BUS_O_WIDTH) ^ GBIT(n, 16, BUS_N_WIDTH))
		|		hash |= 1<<7;
		|	if (GBIT(o, 11, BUS_O_WIDTH) ^ GBIT(n, 17, BUS_N_WIDTH))
		|		hash |= 1<<6;
		|	if (GBIT(o, 15, BUS_O_WIDTH) ^ GBIT(n, 18, BUS_N_WIDTH))
		|		hash |= 1<<5;
		|	if (GBIT(o, 14, BUS_O_WIDTH) ^ GBIT(n, 19, BUS_N_WIDTH))
		|		hash |= 1<<4;
		|	if (GBIT(o, 13, BUS_O_WIDTH) ^ GBIT(n, 20, BUS_N_WIDTH))
		|		hash |= 1<<3;
		|	if (GBIT(o, 12, BUS_O_WIDTH) ^ GBIT(n, 21, BUS_N_WIDTH))
		|		hash |= 1<<2;
		|	if (GBIT(o, 18, BUS_O_WIDTH) ^ GBIT(s, 0, BUS_S_WIDTH))
		|		hash |= 1<<1;
		|	if (GBIT(o, 16, BUS_O_WIDTH) ^ GBIT(s, 2, BUS_S_WIDTH))
		|		hash |= 1<<0;
		|
		|	TRACE(
		|	    << " s " << BUS_S_TRACE()
		|	    << " n " << BUS_N_TRACE()
		|	    << " o " << BUS_O_TRACE()
		|	    << " h " << stdc::hex << hash
		|	);
		|
		|	BUS_HASH_WRITE(hash);
		|
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("XHASH", PartModel("XHASH", XHASH))
