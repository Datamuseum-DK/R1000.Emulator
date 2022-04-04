#!/usr/local/bin/python3
#
# Copyright (c) 2022 Poul-Henning Kamp
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
   Model the NOR components
   =========================
'''

from part import Part, PartFactory

import util

class Nor(PartFactory):

    ''' A generic N-input NOR component '''

    def __init__(self, board, ident, inputs):
        super().__init__(board, ident)
        self.inputs = inputs

    def pin_iterator(self):
        ''' SC pin declarations '''

        for node in self.comp:
            if node.pin.name == "Q" and node.net.sc_type == "bool":
                yield "sc_out <bool>\t\tPIN_Q;"
            elif node.pin.name == "Q":
                yield "sc_out <sc_logic>\tPIN_Q;"
            elif node.net.sc_type == "bool":
                yield "sc_in <bool>\t\tPIN_%s;" % node.pin.name
            else:
                yield "sc_in <sc_logic>\tPIN_%s;" % node.pin.name

    def state(self, file):
        ''' Extra state variable '''

        file.write("\tint job;\n")
        file.write("\tint out;\n")
        file.write("\tunsigned dly;\n")

    def init(self, file):
        ''' Extra initialization '''

        file.fmt('''
		|	state->out = -1;
		|	state->job = 0;
		|	state->dly = 0;
		|''')

    def sensitive(self):
        ''' sensitivity list '''

        for i in range(self.inputs):
            yield "PIN_D%d" % i

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|
		|	TRACE(
		|	    << " j " << state->job
		|	    << " out " << state->out
		|''')

        for i in range(self.inputs):
            file.write("\t    << PIN_D%d\n" % i)

        file.fmt('''
		|	);
		|
		|	if (state->job) {
		|		PIN_Q<=(state->out);
		|		state->job = false;
		|	}
		|
		|''')

        file.write("\n\tif (\n\t    ")

        i = []
        for node in self.comp:
            if node.pin.name == "Q":
                continue
            if node.net.sc_type == "bool":
                i.append("(!PIN_%s.read())" % node.pin.name)
            else:
                i.append("IS_L(PIN_%s.read())" % node.pin.name)
        file.write(" &&\n\t    ".join(i))

        file.fmt('''
		|
		|	) {
		|		if (state->out != 1) {
		|			state->out = 1;
		|			if (state->dly == 0) {
		|				PIN_Q<=(state->out);
		|			} else {
		|				state->job = true;
		|				next_trigger(state->dly, SC_NS);
		|			}
		|		}
		|	} else {
		|		if (state->out != 0) {
		|			state->out = 0;
		|			if (state->dly == 0) {
		|				PIN_Q<=(state->out);
		|			} else {
		|				state->job = true;
		|				next_trigger(state->dly, SC_NS);
		|			}
		|''')

        for node in self.comp:
            if node.pin.name == "Q":
                continue
            if node.net.sc_type == "bool":
                file.write("\t\t} else if (PIN_%s.read()) {\n" % node.pin.name)
            else:
                file.write("\t\t} else if (IS_H(PIN_%s.read())) {\n" % node.pin.name)
            file.write("\t\t\tnext_trigger(PIN_%s.negedge_event());\n" % node.pin.name)

        file.fmt('''
		|		}
		|	}
		|''')

    def hookup(self, file, comp):
        pno = 0
        for node in comp:
            if node.pin.name == "Q":
                file.write("\t%s.PIN_Q(%s);\n" % (comp.name, node.net.cname))
            else:
                file.write("\t%s.PIN_D%d(%s);\n" % (comp.name, pno, node.net.cname))
                pno += 1

class ModelNor(Part):
    ''' Model NOR components '''

    def assign(self, comp):
        ''' Assigned to component '''
        for node in comp:
            if node.pin.name == "Q":
                node.pin.role = "c_output"
            else:
                node.pin.role = "c_input"

    def configure(self, board, comp):
        i = []
        j = 0
        for node in comp:
            if node.pin.name != "Q":
                node.pin.name = "D%d" % j
                j += 1
            if node.net.sc_type == "bool":
                i.append("B")
            else:
                i.append("L")
        inputs = len(comp.nodes) - 1
        sig = util.signature(i)
        ident = "NOR%d_" % inputs + sig
        if ident not in board.part_catalog:
            board.add_part(ident, Nor(board, ident, inputs))
        comp.part = board.part_catalog[ident]

def register(board):
    ''' Register component model '''
    board.add_part("F02", ModelNor("NOR"))
    board.add_part("F260", ModelNor("NOR"))
    board.add_part("NOR4", ModelNor("NOR"))
