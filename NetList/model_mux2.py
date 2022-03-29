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
   Two-choice multiplexers
   =======================
'''


from part import PartModel, PartFactory

class Mux2(PartFactory):

    ''' F15[78] multiplexors '''

    def __init__(self, board, ident, invert):
        super().__init__(board, ident)
        self.invert = invert

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	bool tmp[4];
		|
		|	if (PIN_E=>) {
		|		tmp[0] = false;
		|		tmp[1] = false;
		|		tmp[2] = false;
		|		tmp[3] = false;
		|		// next_trigger(PIN_E.negedge_event());
		|	} else if (PIN_S=>) {
		|		tmp[0] = PIN_B0=>;
		|		tmp[1] = PIN_B1=>;
		|		tmp[2] = PIN_B2=>;
		|		tmp[3] = PIN_B3=>;
		|	} else {
		|		tmp[0] = PIN_A0=>;
		|		tmp[1] = PIN_A1=>;
		|		tmp[2] = PIN_A2=>;
		|		tmp[3] = PIN_A3=>;
		|	}
		|
		|''')

        if self.invert:
            file.fmt('''
		|
		|	tmp[0] = !tmp[0];
		|	tmp[1] = !tmp[1];
		|	tmp[2] = !tmp[2];
		|	tmp[3] = !tmp[3];
		|
		|''')

        file.fmt('''
		|
		|	TRACE (
		|	    << " e " << PIN_E=>
		|	    << " s " << PIN_S=>
		|	    << " a " << PIN_A0=> << PIN_A1=> << PIN_A2=> << PIN_A3=>
		|	    << " b " << PIN_B0=> << PIN_B1=> << PIN_B2=> << PIN_B3=>
		|	    << " | " << tmp[0] << tmp[1] << tmp[2] << tmp[3]
		|	);
		|
		|	PIN_Y0<=(tmp[0]);
		|	PIN_Y1<=(tmp[1]);
		|	PIN_Y2<=(tmp[2]);
		|	PIN_Y3<=(tmp[3]);
		|''')

class ModelMux2(PartModel):
    ''' Model quad two input multiplexers '''

    def __init__(self, invert):
        if invert:
            super().__init__("MUX2I")
        else:
            super().__init__("MUX2")
        self.invert = invert

    def configure(self, board, comp):
        sig = self.make_signature(comp)
        ident = board.name + "_" + self.name + "_" + sig
        if ident not in board.part_catalog:
            board.add_part(ident, Mux2(board, ident, self.invert))
        comp.part = board.part_catalog[ident]

def register(board):
    ''' Register component model '''

    board.add_part("F157", ModelMux2(invert=False))
    board.add_part("F158", ModelMux2(invert=True))
