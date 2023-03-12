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
   F374 Octal D-Type Flip-Flop with 3-STATE Outputs
   ================================================

   Ref: Fairchild DS009524 May 1988 Revised September 2000
'''

from part import PartModel, PartFactory

class Xreg(PartFactory):

    ''' F374 Octal D-Type Flip-Flop with 3-STATE Outputs '''

    def state(self, file):
        ''' Extra state variable '''

        file.write("\tuint64_t data;\n")
        file.write("\tint job;\n")

    def init(self, file):
        ''' Extra initialization '''

        file.fmt('''
		|	state->job = -3;
		|''')

    def sensitive(self):
        ''' sensitivity list '''
        yield "PIN_CLK.pos()"
        if 'OE' in self.comp:
            yield "PIN_OE"

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|
		|	if (state->job == -3) {
		|		BUS_Q_WRITE(0ULL);
		|		state->job = -1;
		|	}
		|
		|	if (state->ctx.do_trace & 2) {
		|		TRACE(
		|			<< " job " << state->job
		|			<< " clk " << PIN_CLK.posedge()
		|''')
        if 'OE' in self.comp:
            file.fmt('''
		|			<< " oe " << PIN_OE?
		|''')

        file.fmt('''
		|			<< " in " << BUS_D_TRACE()
		|			<< " reg " << std::hex << state->data
		|		);
		|	}
		|
		|	if (state->job > 0) {
		|		uint64_t tmp = state->data;
		|''')

        if self.name[-2:] == "_I":
            file.fmt('''
		|		tmp ^= BUS_Q_MASK;
		|''')

        file.fmt('''
		|		TRACE(<< " out " << std::hex << tmp);
		|		BUS_Q_WRITE(tmp);
		|		state->job = 0;
		|''')

        if 'OE' in self.comp:
            file.fmt('''
		|	} else if (state->job == -1) {
		|		BUS_Q_Z();
		|		state->job = -2;
		|''')

        file.fmt('''
		|	}
		|
		|	if (PIN_CLK.posedge()) {
		|		uint64_t tmp = 0;
		|		BUS_D_READ(tmp);
		|''')

        if 'OE' in self.comp:
            file.fmt('''
		|		if (tmp != state->data) {
		|			state->data = tmp;
		|			state->job = -1;
		|		}
		|	}
		|
		|	if (IS_L(PIN_OE)) {
		|		if (state->job < 0) {
		|			state->job = 1;
		|			next_trigger(5, SC_NS);
		|		}
		|	} else if (state->job != -2) {
		|		state->job = -1;
		|		next_trigger(5, SC_NS);
		|	}
		|''')
        else:
            file.fmt('''
		|		if (tmp != state->data) {
		|			state->data = tmp;
		|			state->job = 1;
		|			next_trigger(5, SC_NS);
		|		}
		|	}
		|''')


class ModelXreg(PartModel):
    ''' Xreg registers '''

    def assign(self, comp):

        if 0 and comp.board.name == "VAL" and "ZREG" in comp.name:
            # TEST_Z_CNTR_FILLING.VAL in branch main
            # loop through alu14/15, smux28/29 and xlat7
            comp.part = comp.board.part_catalog["F374_O"]
            return

        oe_node = comp["OE"]
        if oe_node.net.is_pd():
            oe_node.remove()
            for node in comp:
                if node.pin.name[0] == "Q":
                    node.pin.set_role("c_output")
        super().assign(comp)

    def configure(self, board, comp):
        sig = self.make_signature(comp)
        ident = self.name + "_" + sig
        if "INV" in comp and comp["INV"].net.is_pd():
            ident += "_I"
        if ident not in board.part_catalog:
            board.add_part(ident, Xreg(board, ident))
        comp.part = board.part_catalog[ident]

def register(board):
    ''' Register component model '''

    board.add_part("F374", ModelXreg("F374"))
    board.add_part("XREG9", ModelXreg("XREG9"))
    board.add_part("XREG10", ModelXreg("XREG10"))
    board.add_part("XREG14", ModelXreg("XREG14"))
    board.add_part("XREG16", ModelXreg("XREG16"))
    board.add_part("XREG20", ModelXreg("XREG20"))
    board.add_part("XREG24", ModelXreg("XREG24"))
    board.add_part("XREG32", ModelXreg("XREG32"))
    board.add_part("XREG36", ModelXreg("XREG32"))
    board.add_part("XREG64", ModelXreg("XREG64"))
