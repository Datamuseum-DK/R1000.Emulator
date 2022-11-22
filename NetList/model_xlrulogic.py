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
		|	unsigned xd;		// PAR6,49,50,MOD,LRU0,LRU1,LRU2,LRU3 pin13-15
		|	unsigned hd;		// SOIL,LPAR,LRU0,LRU1,LRU2,LRU3,PAR6,HIT
		|	unsigned lrud;		// LRU#~D
		|	unsigned lruupd;	// LRU#~UPD
		|	bool par_d;
		|	bool par_upd;
		|	bool lru_upd_oe;
		|	bool hit_hd;
		|	bool lru_0_oe;
		|	bool lru_1_oe;
		|	bool lpar_qd;
		|	bool soil_qd;
		|	unsigned tag_d;		// -,49,50,MOD,-,-,-,-
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
		|			state->hit_hd = state->hd & 1;
		|			state->tag_d = 0x70 ^ (state->qd & 0x70);
		|			if (state->soil_qd && (!(state->hd & 1)))
		|				state->tag_d &= ~0x10;
		|			state->lru_1_oe = !(
		|				PIN_LRU_UPDATE=> &&
		|				(!PIN_H1=>) &&
		|				(!(state->hd & 1))
		|			);
		|		} else {
		|			// MUXEPAL
		|			state->tag_d = 0x70 ^ state->xd;
		|			if ((state->hd & 0x80) && (!(state->hd & 1)))
		|				state->tag_d &= ~0x10;
		|			state->xd = state->qd & 0x70;
		|			state->lru_0_oe = !(
		|				PIN_LRU_UPDATE=> &&
		|				(!PIN_H1=>) &&
		|				(!hit)
		|			);
		|			state->lru_1_oe = !(
		|				PIN_LRU_UPDATE=> &&
		|				(!PIN_H1=>) &&
		|				(!(state->hd & 1))
		|			);
		|		}
		|		
		|		// LUXXPAL
		|		bool mruisf = PIN_MRUIS_F=>;
		|		unsigned lru = (state->hd & 0x3c) >> 2;
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
		|			case 0x0:
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
		|
		|	if (PIN_LATE=>) {
		|		state->lru_0_oe = !(
		|			(
		|				PIN_LRU_UPDATE=> &&
		|				(!hit)
		|			) || (
		|				PIN_LRU_UPDATE=> &&
		|				PIN_HITQ=>
		|			)
		|		);
		|	}
		|
		|	if (!state->lru_0_oe) {
		|		BUS_HITLRU_WRITE(state->qd & 0x0f);
		|	} else if (!state->lru_1_oe) {
		|		BUS_HITLRU_WRITE((state->hd >> 2) & 0x0f);
		|	} else {
		|		BUS_HITLRU_Z();
		|	}
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
		|
		|	if (state->lru_upd_oe) {
		|		BUS_TAG_Z();
		|	} else {
		|		BUS_TAG_WRITE(hitbus ^ 0xff);
		|	}		
		|
		|	if (PIN_CLK.negedge()) {
		|		state->hd = (state->qd & 0xf) << 2;
		|		state->hd |= (state->qd >> 7) << 1;
		|		if (hit)
		|			state->hd |= 1;
		|		if (state->soil_qd)
		|			state->hd |= 0x80;
		|		if (state->lpar_qd)
		|			state->hd |= 0x40;
		|	}
		|
		|	if ((PIN_LATE=> && PIN_CLK.posedge()) ||
		|	   (!(PIN_LATE=>) && PIN_CLK.negedge())) {
		|
		|		BUS_TAG_READ(state->qd);
		|
		|		// TSXXPAL
		|
		|		unsigned cmd;
		|		BUS_CMD_READ(cmd);
		|
		|		unsigned u = 0;
		|		if (state->qd & 0x01) u++;
		|		if (state->qd & 0x02) u++;
		|		if (state->qd & 0x04) u++;
		|		if (state->qd & 0x08) u++;
		|		state->lpar_qd = u & 1;
		|
		|		if (cmd == 0xd && !PIN_MCYC1=> && !(state->qd & 0x10))
		|			state->soil_qd = true;
		|		else
		|			state->soil_qd = false;
		|	}
		|
		|	if (PIN_LATE=>) {
		|		PIN_TMP0<=(state->hit_hd);
		|	} else {
		|		PIN_TMP0<=(state->hd & 1);
		|	}
		|
		|	TRACE(
		|	    << " lrud " << std::hex << state->lrud
		|	    << " lruupd " << std::hex << state->lruupd
		|	);
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("XLRULOGIC", PartModel("XLRULOGIC", XLRULOGIC))
