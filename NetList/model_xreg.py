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
   Model the F374 chips
   =====================
'''

from component import ModelComponent

class ModelXREG(ModelComponent):
    ''' ... '''

    def __init__(self, *args):
        super().__init__(*args)
        if self.partname == "F374":
            self.width = 8
            self.bus_spec = {
                "D": (0, self.width - 1, "sc_in", True, False),
                "Q": (0, self.width - 1, "sc_out", False, True),
            }
            self.nin = "D"
            self.nout = "Q"
        else:
            self.width = int(self.partname[4:], 10)
            self.bus_spec = {
                "I": (0, self.width - 1, "sc_in", True, False),
                "Y": (0, self.width - 1, "sc_out", False, True),
            }
            self.nin = "I"
            self.nout = "Y"

    def configure(self):
        self.noz = self.nodes["OE"].net.is_pd()
        node = self.nodes.get("INV")
        self.inv = node and node.net.is_pd()

    def is_bus_ok(self, bus):
        retval = super().is_bus_ok(bus)
        if retval and self.noz:
            retval.numeric = True
        return retval

    def make_clsname(self):
        super().make_clsname()
        if self.noz:
            self.clsname += "Z"
        if self.inv:
            self.clsname += "I"

    def hookup_model(self, file):
        ''' ... '''
        self.hookup_bus(file, self.nin)
        self.hookup_bus(file, self.nout)
        self.hookup_pin(file, "PIN_CLK", self.nodes["CLK"])
        if not self.noz:
            self.hookup_pin(file, "PIN_OE", self.nodes["OE"])

    def write_code_hh_signals(self, file):
        file.write('\tsc_in <sc_logic>\tPIN_CLK;\n')
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
        file.write("\n\t    << PIN_CLK.pos()")

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
		|	    << " clk " << PIN_CLK.posedge()
		|'''))

        if not self.noz:
            file.write(self.substitute('''
		|	    << " oe " << PIN_OE
		|'''))

        file.write(self.substitute('''
		|	    << " d "
		|'''))

        for sig in self.iter_signals(self.nin):
            file.write("\t    << %s\n" % sig)

        file.write(self.substitute('''
		|	    << " | " << std::hex << state->data
		|	);
		|
		|	if (state->job > 0) {
		|		uint64_t tmp = state->data ^ state->inv;
		|'''))

        for i in self.write_bus_val(self.nout, "tmp"):
            file.write('\t\t' + i + '\n')

        file.write(self.substitute('''
		|		state->job = 0;
		|'''))


        if not self.noz:
            file.write(self.substitute('''
		|	} else if (state->job == -1) {
		|'''))
            for i in self.write_bus_z(self.nout):
                file.write('\t\t' + i + '\n')

            file.write(self.substitute('''
		|		state->job = -2;
		|'''))

        file.write(self.substitute('''
		|	}
		|
		|	if (PIN_CLK.posedge()) {
		|		uint64_t tmp = 0;
		|'''))

        for i in self.read_bus_value("tmp", self.nin):
            file.write("\t\t" + i + "\n")

        file.write(self.substitute('''
		|		if (tmp != state->data) {
		|			state->data = tmp;
		|			state->job = -1;
		|		}
		|	}
		|
		|'''))

        if not self.noz:
            file.write(self.substitute('''
		|	if (IS_L(PIN_OE)) {
		|		if (state->job < 0) {
		|			state->job = 1;
		|			next_trigger(5, SC_NS);
		|		}
		|	} else if (state->job != -2) {
		|		state->job = -1;
		|		next_trigger(5, SC_NS);
		|	}
		|}
		|'''))
        else:
            file.write(self.substitute('''
		|	if (state->job) {
		|		state->job = 1;
		|		next_trigger(5, SC_NS);
		|	}
		|}
		|'''))
