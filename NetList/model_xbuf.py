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
   Model the XBUF%d
   ================
'''

from component import ModelComponent

class ModelXBUF(ModelComponent):
    ''' XBUF%d '''

    def __init__(self, board, sexp):
        super().__init__(board, sexp)
        self.width = int(self.partname[4:], 10)
        # print("XBUF", self.width)
        self.bus_spec = {
            "I": (0, self.width - 1, "sc_in", True, False),
            "Y": (0, self.width - 1, "sc_out", False, True),
        }

    def configure(self):
        self.noz = self.nodes["OE"].net.is_pd()
        node = self.nodes.get("INV")
        if node:
            self.inv = node.net.is_pd()
        else:
            libsource = self.sexp.find_first("libsource")
            self.inv = libsource[2][0].name == "F240"
            # print("XBUF/F24x", self.width, self, " ".join(libsource.serialize()))

    def make_clsname(self):
        super().make_clsname()
        if self.fallback:
            return
        if not self.noz:
            self.clsname += "Z"
        if self.inv:
            self.clsname += "I"

    def is_bus_ok(self, bus):
        self.configure()
        retval = super().is_bus_ok(bus)
        if retval and self.noz:
            retval.numeric = True
        return retval

    def hookup_model(self, file):
        ''' ... '''
        if self.fallback:
            super().hookup(file)
            return
        self.hookup_bus(file, "I")
        self.hookup_bus(file, "Y")
        if not self.noz:
            self.hookup_pin(file, "PIN_OE", self.nodes["OE"])

    def write_code_hh_signals(self, file):
        if not self.noz:
            file.write('\tsc_in <sc_logic>\tPIN_OE;\n')

    def write_code_cc_state_extra(self, file):
        file.write("\tuint64_t inv;\n")

    def write_code_cc_init_extra(self, file):
        file.write("\tstate->job = -1;\n")
        if self.inv:
            file.write("\tstate->inv = 0x%xULL;\n" % ((1<<self.width) -1))

    def write_code_cc_sensitive(self, file):
        if not self.noz:
            file.write("\n\t    << PIN_OE")
        self.write_sensitive_bus(file, "I")

    def write_code_cc(self, file):
        self.write_code_cc_init(file)
        file.write(self.substitute('''
		|void
		|SCM_MMM :: doit(void)
		|{
		|	state->ctx.activations++;
		|
		|	TRACE(
		|	    << " job " << state->job
		|'''))

        if not self.noz:
            file.write(self.substitute('''
		|	    << " oe " << PIN_OE
		|	    << " i "
		|'''))

        for sig in self.iter_signals("I"):
            file.write("\t    << %s\n" % sig)

        file.write(self.substitute('''
		|	);
		|
		|	if (state->job > 0) {
		|		uint64_t tmp = state->data ^ state->inv;
		|'''))

        for i in self.write_bus_val("Y", "tmp"):
            file.write('\t\t' + i + '\n')

        file.write(self.substitute('''
		|		state->job = 0;
		|'''))

        if not self.noz:
            file.write(self.substitute('''
		|	} else if (state->job == -1) {
		|'''))

            for i in self.write_bus_z("Y"):
                file.write('\t\t' + i + '\n')

            file.write(self.substitute('''
		|		state->job = -2;
		|		next_trigger(PIN_OE.negedge_event());
		|	}
		|
		|	if (IS_H(PIN_OE)) {
		|		if (state->job != -2) {
		|			state->job = -1;
		|			next_trigger(5, SC_NS);
		|		} else {
		|			next_trigger(PIN_OE.negedge_event());
		|		}
		|		return;
		|	}
		|	if (state->job < 0) {
		|		state->job = 1;
		|		next_trigger(5, SC_NS);
		|'''))

        file.write(self.substitute('''
		|	}
		|
		|	uint64_t i_val = 0;
		|'''))

        for i in self.read_bus_value("i_val", "I"):
            file.write("\t" + i + "\n")

        file.write(self.substitute('''
		|
		|	if (i_val != state->data) {
		|		state->data = i_val;
		|		state->job = 1;
		|		next_trigger(5, SC_NS);
		|	}
		|}
		|'''))
