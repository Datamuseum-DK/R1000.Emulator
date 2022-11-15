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
   LRU logic
   =========

'''


from part import PartModel, PartFactory

class XLRULOGIC(PartFactory):

    ''' The LRU logic'''

    def state(self, file):
        file.fmt('''
		|	unsigned qd;		// PAR6,49,50,MOD,LRU0,LRU1,LRU2,LRU3
		|	unsigned hd;		// SOIL,LPAR,LRU0,LRU1,LRU2,LRU3,PAR6,HIT
		|	unsigned lrud;		// LRU#~D
		|	unsigned lruupd;	// LRU#~UPD
		|	unsigned luxx;		// LRU#~D,PAR~D,PAR~UPD,LRU#~UPD
		|''')

    def sensitive(self):
        yield "PIN_CLK"
        yield "PIN_HITQ"
        yield "PIN_NMATCH"
        yield "PIN_OMATCH"
        yield "PIN_LOGQ"
        yield "BUS_LHIT_SENSITIVE()"

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	bool hit;
		|
		|	if (PIN_HITQ=>) {
		|		hit = false;
		|	} else if (PIN_NMATCH=> && PIN_OMATCH=> && PIN_LOGQ=>) {
		|		hit = false;
		|	} else {
		|		hit = true;
		|	}
		|	PIN_HIT<=(hit);
		|
		|	if (PIN_CLK.posedge()) {
		|		// LUXXPAL
		|		bool mruisf = PIN_MRUIS_F=>;
		|		state->luxx = 0;
		|		if (state->hd & 0x01) {
		|			state->lrud = (state->hd & 0x3c) >> 2;
		|			state->lruupd = ( (state->hd & 0x3c) >> 2) - 1;
		|		} else if (mruisf) {
		|			state->lrud = 0xf;
		|			state->lruupd = 0xf;
		|		} else {
		|			state->lrud = 0x7;
		|			state->lruupd = 0x7;
		|		}
		|
		|		state->luxx = ((~state->lrud) & 0xf) << 6;
		|		state->luxx = ((~state->lruupd) & 0xf);
		|	}
		|
		|	unsigned lhit;
		|	BUS_LHIT_READ(lhit);
		|	if ((0x0f ^ state->lrud) < lhit)
		|		state->luxx |= 1 << 10;
		|	else
		|		state->luxx &= ~(1 << 10);
		|
		|	if (PIN_CLK.negedge()) {
		|		state->hd = (state->qd & 0xf) << 2;
		|		state->hd |= (state->qd >> 7) << 1;
		|		if (hit)
		|			state->hd |= 1;
		|		if (PIN_SOIL=>)
		|			state->hd |= 0x80;
		|		if (PIN_LPAR=>)
		|			state->hd |= 0x40;
		|	}
		|
		|	if (PIN_LATE=> && PIN_CLK.posedge()) {
		|		BUS_TAG_READ(state->qd);
		|	} else if (!(PIN_LATE=>) && PIN_CLK.negedge()) {
		|		BUS_TAG_READ(state->qd);
		|	}
		|
		|	// 49_Q,50_Q,MOD_Q,SOIL_H,LRU#_H,PAR6_H,HIT_H,LRU#_Q
		|	unsigned tmp = state->qd & 0x0f;
		|	tmp |= state->hd << 4;
		|	tmp |= (state->qd & 0x70) << 8;
		|	tmp |= state->luxx << 15;
		|	BUS_TMP_WRITE(tmp);
		|
		|	BUS_TAG_Z();
		|	BUS_HITLRU_Z();
		|	TRACE(
		|	    << " lrud " << std::hex << state->lrud
		|	    << " lruupd " << std::hex << state->lruupd
		|	    << " luxx " << std::hex << state->luxx
		|	    << " tmp " << std::hex << tmp
		|	);
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("XLRULOGIC", PartModel("XLRULOGIC", XLRULOGIC))
