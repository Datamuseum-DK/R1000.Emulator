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
   F24[04] Octal buffers (3-state)
   ===============================

   Ref: Philips IC15 1994 Dec 05
'''

from part import PartModel, PartFactory
from component import Component

class Xbuf(PartFactory):

    ''' F24[04] Octal buffers (3-state) '''

    def __init__(self, board, ident, invert):
        super().__init__(board, ident)
        self.invert = invert
        self.bits = 0

    def state(self, file):
        ''' Extra state variable '''

        file.write("\tuint64_t data;\n")
        file.write("\tint job;\n")

    def init(self, file):
        ''' Extra initialization '''

        file.fmt('''
		|	state->job = -1;
		''')

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        for node in self.comp:
            if node.pin.name[0] == 'I':
                self.bits += 1

        file.fmt('''
		|	uint64_t tmp;
		|
		|	if (state->job > 0) {
		|		tmp = state->data;
		|''')

        if self.invert:
            file.fmt('''
		|		tmp = ~tmp;
		|''')

        for node in self.comp:
            if node.pin.name[0] == 'Y':
                i = self.bits - int(node.pin.name[1:]) - 1
                file.fmt("\n\t\t|\t\tPIN_%s<=(tmp & ((uint64_t)1 << %d));\n\n" % (node.pin.name, i))

        file.fmt('''
		|		state->job = 0;
		|''')

        if "OE" not in self.comp:
            file.fmt('''
		|	}
		|
		|	if (true) {
		|		tmp = 0;
		|''')
        else:
            file.fmt('''
		|	} else if (state->job < 0) {
		|''')

            for node in self.comp:
                if node.pin.name[0] == 'Y':
                    file.write("\t\tPIN_%s = sc_logic_Z;\n" % node.pin.name)

            file.fmt('''
		|	}
		|
		|	if (PIN_OE=>) {
		|''')

            for node in self.comp:
                if node.pin.name[0] == 'Y':
                    file.write("\t\tPIN_%s = sc_logic_Z;\n" % node.pin.name)

            file.fmt('''
		|		state->job = -1;
		|		next_trigger(PIN_OE.negedge_event());
		|	} else {
		|		tmp = 0;
		|''')

        for node in self.comp:
            if node.pin.name[0] != 'I':
                continue
            i = self.bits - int(node.pin.name[1:]) - 1
            file.fmt(
                "\n\t\t|\t\tif (PIN_%s=>) tmp |= ((uint64_t)1 << %d);\n\n" % (
                    node.pin.name,
                    i,
                )
            )

        file.fmt('''
		|		if (tmp != state->data || state->job < 0) {
		|			state->data = tmp;
		|			state->job = 1;
		|			next_trigger(5, SC_NS);
		|		}
		|	}
		|''')

class ModelXbuf(PartModel):
    ''' Xbuf registers '''

    def __init__(self, invert):
        super().__init__("XBUF")
        self.invert = invert

    def assign(self, comp):
        for node in comp:
            if node.pin.name[0] != 'Y':
                node.pin.role = "c_input"
        oenode0 = comp["OE0"]
        oenode1 = comp["OE1"]
        if oenode0.net == oenode1.net:
            oenode1.remove()
            oenode0.remove()
            if oenode0.net.is_pd():
                print("XBUF-8-PD", comp)
                for node in comp:
                    if node.pin.name[0] == 'Y':
                        node.pin.role = "c_output"
            else:
                print("XBUF-8-VAR", comp)
                oenode0.pin.name = "OE"
                oenode0.insert()
            return

        comp2 = Component(
            comp.board,
            comp.ref + "_B",
            comp.value,
            comp.sheet,
            comp.partname,
        )
        comp2.name = comp.name + "_B"
        comp2.location = comp.location
        comp2.part = self
        for src, dst in (
            ("I4", "I0"),
            ("I5", "I1"),
            ("I6", "I2"),
            ("I7", "I3"),
            ("Y4", "Y0"),
            ("Y5", "Y1"),
            ("Y6", "Y2"),
            ("Y7", "Y3"),
        ):
            node = comp[src]
            node.remove()
            node.pin.name = dst
            node.component = comp2
            node.insert()

        oenode0.remove()
        oenode0.pin.name = "OE"
        oenode0.insert()

        oenode1.remove()
        oenode1.pin.name = "OE"
        oenode1.component = comp2
        oenode1.insert()

        for this in (comp, comp2):
            node = this["OE"]
            if node.net.is_pd():
                print("XBUF-4-PD", this)
                node.remove()
                for node in this:
                    if node.pin.name[0] == "Y":
                        node.pin.role = "c_output"
            elif len(node.net) == 1 or node.net.is_pu():
                print("XBUF-4-NOOP", this)
                for node in this:
                    node.remove()
                this.remove()
            else:
                print("XBUF-4-VAR", this)

    def configure(self, board, comp):
        sig = self.make_signature(comp)
        ident = board.name + "_" + self.name + "_" + sig
        if self.invert:
            ident += "_I"
        if ident not in board.part_catalog:
            board.add_part(ident, Xbuf(board, ident, self.invert))
        comp.part = board.part_catalog[ident]

def register(board):
    ''' Register component model '''

    board.add_part("F244", ModelXbuf(False))
    board.add_part("F240", ModelXbuf(True))
