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
   2Kx8 SRAM
   =========

'''


from part import PartModel, PartFactory

class SRAM16KX4(PartFactory):

    ''' 16Kx8 SRAM '''

    def state(self, file):
        file.fmt('''
		|	uint8_t ram[1<<14];
		|''')

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	unsigned adr = 0;
		|	unsigned data;
		|	const char *what = NULL;
		|
		|	BUS_A_READ(adr);
		|	data = state->ram[adr];
		|	if (PIN_CS.posedge()) {
		|		BUS_IO_Z();
		|		TRACE("Z");
		|		next_trigger(PIN_CS.negedge_event());
		|		return;
		|	}
		|	if (PIN_CS.negedge()) {
		|		if (!PIN_WE=>) {
		|			BUS_IO_Z();
		|			what = " Ew ";
		|		} else {
		|			what = " Er ";
		|		}
		|	}
		|	if (PIN_WE.negedge()) {
		|		BUS_IO_Z();
		|		what = " w ";
		|	}
		|	if (PIN_WE.posedge()) {
		|		BUS_IO_READ(data);
		|		state->ram[adr] = data;
		|		what = " W ";
		|	}
		|	if (!PIN_CS=> && PIN_WE=>) {
		|		BUS_IO_WRITE(data);
		|		if (state->ctx.do_trace & 2)
		|			what = " R ";
		|	}
		|	if (what != NULL) {
		|		TRACE(
		|		    << what
		|		    << " cs_ " << PIN_CS?
		|		    << " we_ " << PIN_WE?
		|		    << " a " << BUS_A_TRACE()
		|		    << " d " << BUS_IO_TRACE()
		|		    << " A " << std::hex << adr
		|		    << " D " << std::hex << data
		|		);
		|	}
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("16KX4", PartModel("16KX4", SRAM16KX4))