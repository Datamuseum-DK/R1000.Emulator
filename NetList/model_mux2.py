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

from component import ModelComponent

class ModelMux2(ModelComponent):
    ''' Two-choice multiplexers '''

    def __init__(self, board, sexp):
        super().__init__(board, sexp)
        if self.partname == "XMUX16":
            self.width = 16
        else:
            self.width = 4
        self.inv = 0
        self.oe = False
        self.bus_spec = {
            "A": [0, self.width - 1, "sc_in", True, False],
            "B": [0, self.width - 1, "sc_in", True, False],
            "Y": [0, self.width - 1, "sc_out", False, True],
        }
        if self.partname == "XMUX16":
            print("XX", self)
        elif self.partname == "F157":
            self.bus_spec["Y"][3] = True
        elif self.partname == "F158":
            self.bus_spec["Y"][3] = True
            self.inv = (1<<self.width) - 1
        elif self.partname == "F258":
            self.inv = (1<<self.width) - 1
            self.oe = True
        else:
            assert self.partname == "F257"
            self.oe = True

    def configure(self):
        node = self.nodes.get("INV")
        if node and node.net.is_pd():
            self.inv = (1<<self.width) - 1
        node = self.nodes.get("OE")
        if node and node.net.is_pd():
            self.oe = False

    def make_clsname(self):
        super().make_clsname()
        if not self.oe:
            self.clsname += "Z"
        if self.inv:
            self.clsname += "I"

    def hookup_model(self, file):
        ''' ... '''
        if self.fallback:
            print("TTT", self.partname, self.nodes["OE"].net)
            super().hookup(file)
            return
        self.hookup_bus(file, "A")
        self.hookup_bus(file, "B")
        self.hookup_bus(file, "Y")
        self.hookup_pin(file, "PIN_S", self.nodes["S"])
        if self.oe:
            self.hookup_pin(file, "PIN_OE", self.nodes["OE"])
        else:
            self.hookup_pin(file, "PIN_E", self.nodes["E"])

    def write_code_hh_signals(self, file):
        file.write('\tsc_in <sc_logic>\tPIN_S;\n')
        if self.oe:
            file.write('\tsc_in <sc_logic>\tPIN_OE;\n')
        else:
            file.write('\tsc_in <sc_logic>\tPIN_E;\n')

    def write_code_cc_sensitive(self, file):
        if self.oe:
            file.write("\n\t    << PIN_OE")
        else:
            file.write("\n\t    << PIN_E")
        file.write("\n\t    << PIN_S")
        self.write_sensitive_bus(file, "A")
        self.write_sensitive_bus(file, "B")

    def write_code_cc(self, file):
        self.write_code_cc_init(file)
        file.write(self.substitute('''
		|void
		|SCM_MMM :: doit(void)
		|{
		|	uint64_t tmp = 0;
		|
		|	state->ctx.activations++;
		|
		|'''))

        if self.oe:
            file.write(self.substitute('''
		|	if (IS_H(PIN_OE)) {
		|'''))
            for i in self.write_bus_z("Y"):
                file.write('\t\t' + i + '\n')
            file.write(self.substitute('''
		|		next_trigger(PIN_OE.negedge_event());
		|		return;
		|'''))

        else:

            file.write(self.substitute('''
		|	if (IS_H(PIN_E)) {
		|'''))

            for i in self.write_bus_val("Y", "0x%xULL" % self.inv):
                file.write('\t\t' + i + '\n')

            file.write(self.substitute('''
		|		next_trigger(PIN_E.negedge_event());
		|'''))

        file.write(self.substitute('''
		|	} else if (IS_H(PIN_S)) {
		|'''))

        for i in self.read_bus_value("tmp", "B"):
            file.write("\t\t" + i + "\n")

        file.write(self.substitute('''
		|	} else {
		|'''))

        for i in self.read_bus_value("tmp", "A"):
            file.write("\t\t" + i + "\n")

        file.write(self.substitute('''
		|	}
		|
		|'''))

        if self.inv:
            file.write("\ttmp ^= 0x%xULL;\n\n" % self.inv)

        for i in self.write_bus_val("Y", "tmp"):
            file.write('\t' + i + '\n')

        file.write(self.substitute('''
		|}
		|'''))
