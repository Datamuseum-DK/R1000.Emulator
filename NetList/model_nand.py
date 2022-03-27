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
   Model the NAND components
   =========================
'''

from part import Part

class Nand(Part):

    ''' A generic N-input NAND component '''

    def __init__(self, board, inputs):
        super().__init__('%s_NAND%d' % (board.name, inputs))
        self.board = board
        self.inputs = inputs
        self.scm = False

    def build(self):
        ''' Build this component (if/when used) '''

        self.scm = self.board.sc_mod(self.name)
        self.scm.std_hh(self.name, self.pin_iterator())
        self.scm.std_cc(
            self.name,
            self.state,
            self.init,
            self.sensitive,
            self.doit
        )
        self.scm.commit()
        self.board.extra_scms.append(self.scm)

    def yield_includes(self, _comp):
        ''' (This is the first call we get when used '''

        if not self.scm:
            self.build()
        yield self.scm.hh.filename

    def pin_iterator(self):
        ''' SC pin declarations '''

        for i in range(self.inputs):
            yield "sc_in <sc_logic>\tPIN_D%d;" % i
        yield "sc_out <sc_logic>\tPIN_Q;"

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
		|	state->dly = 5;
		|	if (strstr(this->name(), "IOC.ioc_54.RDNAN0A") != NULL) {
		|		// TEST_MACRO_EVENT_SLICE.IOC @ optimized
		|		state->dly = 10;
		|	}
		|	if (strstr(this->name(), "IOC.ioc_54.RDNAN0B") != NULL) {
		|		// TEST_MACRO_EVENT_DELAY.IOC @ optimized
		|		state->dly = 10;
		|	}
		|	if (strstr(this->name(), "TYP.typ_40.CKDR5A") != NULL) {
		|		// TEST_LOOP_CNTR_OVERFLOW.TYP @ main
		|		state->dly = 2;
		|	}
		|''')

    def sensitive(self):
        ''' sensitivity list '''

        for i in range(self.inputs):
            yield "PIN_D%d" % i

    def doit(self, file):
        ''' The meat of the doit() function '''

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
		|		PIN_Q = AS(state->out);
		|		state->job = false;
		|	}
		|
		|''')

        file.write("\n\tif (\n\t    ")
        file.write(" &&\n\t    ".join("IS_H(PIN_D%d)" % x for x in range(self.inputs)))
        file.fmt('''
		|
		|	) {
		|		if (state->out != 0) {
		|			state->out = 0;
		|			if (state->dly == 0) {
		|				PIN_Q = AS(state->out);
		|			} else {
		|				state->job = true;
		|				next_trigger(state->dly, SC_NS);
		|			}
		|		}
		|	} else {
		|		if (state->out != 1) {
		|			state->out = 1;
		|			if (state->dly == 0) {
		|				PIN_Q = AS(state->out);
		|			} else {
		|				state->job = true;
		|				next_trigger(state->dly, SC_NS);
		|			}
		|''')

        for i in range(self.inputs):
            file.write("\t\t} else if (IS_L(PIN_D%d)) {\n" % i)
            file.write("\t\t\tnext_trigger(PIN_D%d.posedge_event());\n" % i)

        file.fmt('''
		|		}
		|	}
		|''')

    def hookup(self, file, comp):
        pno = 0
        for node in comp.iter_nodes():
            if node.pin.name == "Q":
                file.write("\t%s.PIN_Q(%s);\n" % (comp.name, node.net.cname))
            else:
                file.write("\t%s.PIN_D%d(%s);\n" % (comp.name, pno, node.net.cname))
                pno += 1

class ModelNand(Part):
    ''' Model NAND components '''

    def __init__(self):
        super().__init__("NAND")

    def configure(self, board, comp):
        #print("NAND", comp)
        inputs = len(comp.nodes) - 1
        ident = "NAND%d" % inputs
        comp.part = board.part_catalog[ident]

def register(board):
    ''' Register component model '''

    for i in (1, 2, 3, 4, 13):
        board.add_part("NAND{}".format(i), Nand(board, i))
    board.add_part("F04", ModelNand())	# Inverters are juvenile NAND gates
    board.add_part("F37", ModelNand())
    #board.add_part("F10", ModelNand())
    # board.add_part("F20", ModelNand())   ### Not: OC-thing with ALU-ZERO outputs
    #board.add_part("F30", ModelNand())
    board.add_part("F40", ModelNand())
    board.add_part("F133", ModelNand())
