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
   128KX4X9 SRAM
   =============

'''


from part import PartModel, PartFactory

class XIOPRAM(PartFactory):

    ''' 128KX4X9 SRAM '''

    def state(self, file):
        file.fmt('''
		|	uint32_t *ram;
		|''')

    def sensitive(self):
        yield "PIN_CS"

    def init(self, file):

        super().init(file)

        file.fmt('''
		|	struct ctx *c1 = CTX_Find("IOP.ram_space iop_ram_space");
		|	assert(c1 != NULL);
		|	state->ram = (uint32_t*)(c1 + 1);
		|''')

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	TRACE(
		|	    << " a " << BUS_A_TRACE()
		|	    << " d " << BUS_D_TRACE()
		|	    << " we " << BUS_WE_TRACE()
		|	    << " cs " << PIN_CS?
		|	);
		|''')
        return

        file.fmt('''
		|	unsigned adr;
		|	BUS_A_READ(adr);
		|
		|	if (!PIN_WE=>) {
		|		BUS_D_READ(state->ram[adr]);
		|		TRACE(
		|		    << " w a " << BUS_A_TRACE()
		|		    << " d " << BUS_D_TRACE()
		|		    << " we "
		|		    << PIN_WE?
		|		    << " adr "
		|		    << std::hex << adr
		|		    << " data "
		|		    << std::hex << (unsigned)state->ram[adr]
		|		);
		|		next_trigger(
		|		    PIN_WE.posedge_event() | BUS_D_EVENTS()
		|		);
		|	} else {
		|		TRACE(
		|		    << " r a " << BUS_A_TRACE()
		|		    << " d "
		|		    <<AS(state->ram[adr] & 0x80)
		|		    <<AS(state->ram[adr] & 0x40)
		|		    <<AS(state->ram[adr] & 0x20)
		|		    <<AS(state->ram[adr] & 0x10)
		|		    <<AS(state->ram[adr] & 0x08)
		|		    <<AS(state->ram[adr] & 0x04)
		|		    <<AS(state->ram[adr] & 0x02)
		|		    <<AS(state->ram[adr] & 0x01)
		|		    << " we "
		|		    << PIN_WE?
		|		    << " adr "
		|		    << std::hex << adr
		|		    << " data "
		|		    << std::hex << (unsigned)state->ram[adr]
		|		);
		|	}
		|	BUS_Q_WRITE(state->ram[adr]);
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("XIOPRAM4", PartModel("XIOPRAM4", XIOPRAM))
    board.add_part("XIOPRAM32", PartModel("XIOPRAM32", XIOPRAM))
