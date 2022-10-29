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
   REGISTER FILES
   ==============

'''

from part import PartModel, PartFactory

class XRFTB(PartFactory):

    ''' TYP RF B '''

    def state(self, file):
        file.fmt('''
		|	uint64_t ram[1<<BUS_AW_WIDTH];
		|''')

    def sensitive(self):
         yield "PIN_WE.pos()"
         # yield "PIN_CS"
         #yield "BUS_AW_SENSITIVE()"
         #yield "BUS_AR_SENSITIVE()"
         yield "PIN_RD.pos()"

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	unsigned adr = 0, b = 0, a2;
		|	uint64_t data = 0;
		|	const char *what = "?";
		|
		|	if (PIN_WE.posedge() && !PIN_CS=>) {
		|		BUS_D_READ(data);
		|		BUS_AW_READ(adr);
		|		state->ram[adr] = data;
		|		what = "W";
		|	} else if (PIN_WE.posedge()) {
		|		printf("TYP supressed write\\n");
		|	} else {
		|		BUS_B_READ(b);
		|		if (b == 0x29 && PIN_TRCV=>) {
		|			BUS_TYP_READ(data);
		|			data ^= BUS_TYP_MASK;
		|			what = "T";
		|			next_trigger(PIN_WE.posedge_event() | PIN_RD.default_event() | BUS_TYP_EVENTS());
		|		} else if (b == 0x2c) {
		|			BUS_CNT_READ(adr);
		|			data = state->ram[adr];
		|			what = "C";
		|		} else {
		|			if (!(b & 0x20)) {
		|				BUS_FRM_READ(adr);
		|				adr <<= 5;
		|			}
		|			adr |= b & 0x10;
		|			if ((b & 0x30) != 0x20) {
		|				adr |= (b & 0xf);
		|			} else if ((b & 0x3c) == 0x28) {
		|				BUS_CSA_READ(a2);
		|				adr |= a2;
		|			} else {
		|				BUS_TOS_READ(a2);
		|				adr |= a2;
		|			}
		|			data = state->ram[adr];
		|			what = "R";
		|		}
		|		BUS_Q_WRITE(data);
		|	}
		|
		|	TRACE(
		|	    << what
		|	    << " cs " << PIN_CS
		|	    << " we^ " << PIN_WE.posedge()
		|	    << " aw " << BUS_AW_TRACE()
		|	    << " rd^ " << PIN_RD.posedge()
		|	    << " b " << BUS_B_TRACE()
		|	    << " frm " << BUS_FRM_TRACE()
		|	    << " csa " << BUS_CSA_TRACE()
		|	    << " tos " << BUS_TOS_TRACE()
		|	    << " cnt " << BUS_CNT_TRACE()
		|	    << " trcv " << PIN_TRCV
		|	    << " typ " << BUS_TYP_TRACE()
		|	    << " d " << BUS_Q_TRACE()
		|	);
		|''')


class XRFVB(PartFactory):

    ''' VAL RF B '''

    def state(self, file):
        file.fmt('''
		|	uint64_t ram[1<<BUS_AW_WIDTH];
		|''')

    def sensitive(self):
        yield "PIN_WE.pos()"
        yield "PIN_RD.pos()"

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	unsigned adr = 0, a2;
		|	unsigned b = 0;
		|	uint64_t data = 0;
		|	const char *what = "?";
		|
		|	if (!PIN_CS=>) {
		|		BUS_B_READ(b);	// NB: inverted
		|
		|		if (b < 0x20) {
		|			BUS_FRM_READ(adr);
		|			adr <<= 5;
		|		}
		|
		|		adr |= b & 0x10;
		|
		|		if ((b & 0x30) != 0x20) {
		|			adr |= b & 0x0f;
		|		} else if ((b & 0x2c) == 0x28) {
		|			BUS_CSA_READ(a2);
		|			adr |= a2;
		|		} else if (b == 0x2c) {
		|			BUS_CNT_READ(adr);
		|		} else {
		|			BUS_TOS_READ(a2);
		|			adr |= a2;
		|		}
		|
		|		data = state->ram[adr] ^ BUS_Q_MASK;
		|		BUS_Q_WRITE(data);
		|		what = "r";
		|	}
		|
		|	if (PIN_WE.posedge() && !PIN_CS=>) {
		|		BUS_D_READ(data);
		|		BUS_AW_READ(adr);
		|		state->ram[adr] = data;
		|		what = "w";
		|	} else if (PIN_WE.posedge()) {
		|		printf("VAL supressed write\\n");
		|	}
		|
		|	TRACE(
		|	    << what
		|	    << " cs " << PIN_CS?
		|	    << " we^ " << PIN_WE.posedge()
		|	    << " aw " << BUS_AW_TRACE()
		|	    << " dw " << BUS_D_TRACE()
		|	    << " rd^ " << PIN_RD.posedge()
		|	    << " b " << BUS_B_TRACE()
		|	    << " frm " << BUS_FRM_TRACE()
		|	    << " csa " << BUS_CSA_TRACE()
		|	    << " btos " << BUS_TOS_TRACE()
		|	    << " cnt " << BUS_CNT_TRACE()
		|	    << " q " << BUS_Q_TRACE()
		|	);
		|''')


def register(board):
    ''' Register component model '''

    board.add_part("XRFTB", PartModel("XRFTB", XRFTB))
    board.add_part("XRFVB", PartModel("XRFVB", XRFVB))
