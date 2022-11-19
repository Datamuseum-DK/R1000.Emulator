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
		|	bool par_d;
		|	bool par_upd;
		|	bool lru_upd_oe;
		|	unsigned tag_d;		// -49,50,MOD,-,-,-,-
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
		|	unsigned hitbus = 0;
		|	unsigned tmp = 0;
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
		|		// MUXxPAL
		|		state->lru_upd_oe = !PIN_LRU_UPDATE;
		|
		|		if (PIN_LATE=>) {
		|			// MUXLPAL
		|			state->tag_d = 0x70 ^ (state->qd & 0x70);
		|			if (PIN_SOIL=> && (!(state-> hd & 1)))
		|				state->tag_d &= ~0x10;
		|		}
		|		
		|		// LUXXPAL
		|		bool mruisf = PIN_MRUIS_F=>;
		|		unsigned lru = (state->hd & 0x3c) >> 2;
		|		state->luxx = 0;
		|		if (state->hd & 0x01) {
		|			state->lrud = lru;
		|			if (lru > 1)
		|				state->lruupd = lru - 1;
		|			else
		|				state->lruupd = 0x0;
		|		} else if (mruisf) {
		|			state->lrud = 0xf;
		|			state->lruupd = 0xf;
		|		} else {
		|			state->lrud = 0x7;
		|			state->lruupd = 0x7;
		|		}
		|
		|		unsigned par_d = state->hd >> 1; // PAR6
		|		if (!(state->hd & 0x01)) {
		|			if (state->hd & 0x80) par_d++;	// SOIL
		|			if (state->hd & 0x40) par_d++;	// LPAR
		|			if (mruisf) par_d++;
		|			par_d += 1;
		|		}
		|		state->par_d = !(par_d & 1);
		|
		|		unsigned par_upd = 0;
		|		if (!(state->hd & 0x01)) {
		|			if (state->hd & 0x02) par_upd++;	// PAR6
		|			if (state->hd & 0x80) par_upd++;	// SOIL
		|			if (state->hd & 0x40) par_upd++;	// LPAR
		|			if (mruisf) par_upd++;
		|			par_upd += 1;
		|		} else {
		|			switch(lru) {
		|			case 0x2:
		|			case 0x6:
		|			case 0x8:
		|			case 0xa:
		|			case 0xe:
		|				par_upd = state->hd >> 1;
		|				break;
		|			case 0x1:
		|			case 0x3:
		|			case 0x4:
		|			case 0x5:
		|			case 0x7:
		|			case 0x9:
		|			case 0xb:
		|			case 0xc:
		|			case 0xd:
		|			case 0xf:
		|				par_upd = 1 ^ (state->hd >> 1);
		|				break;
		|			}
		|		}
		|		state->par_upd = !(par_upd & 1);
		|
		|	}
		|	tmp |= state->luxx << 15;
		|
		|	if (state->lru_upd_oe)
		|		tmp |= 1 << 25;
		|
		|	unsigned lhit;
		|	BUS_LHIT_READ(lhit);
		|	if ((0x0f ^ state->lrud) < lhit) {
		|		hitbus |= (~state->lruupd) & 0xf;
		|		if (state->par_upd) {
		|			hitbus |= 1 << 7;
		|		}
		|	} else {
		|		hitbus |= (~state->lrud) & 0xf;
		|		if (state->par_d) {
		|			hitbus |= 1 << 7;
		|		}
		|	}
		|	hitbus |= state->tag_d;
		|	tmp |= hitbus << 15;
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
		|	tmp |= state->hd << 4;
		|
		|	if (PIN_LATE=> && PIN_CLK.posedge()) {
		|		BUS_TAG_READ(state->qd);
		|	} else if (!(PIN_LATE=>) && PIN_CLK.negedge()) {
		|		BUS_TAG_READ(state->qd);
		|	}
		|	tmp |= state->qd & 0x0f;
		|	tmp |= (state->qd & 0x70) << 8;
		|
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
