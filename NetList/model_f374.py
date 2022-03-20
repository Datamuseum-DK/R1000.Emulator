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
        self.width = 8
        self.bus_spec = {
            "D": (0, self.width - 1, "sc_in"),
            "Q": (0, self.width - 1, "sc_out"),
        }
        super().__init__(*args)

    def write_code_hh_signals(self, file):
        file.write('\tsc_in <sc_logic>\tPIN_CLK;\n')
        file.write('\tsc_in <sc_logic>\tPIN_OE;\n')

    def write_code_cc_sensitive(self, file):
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
		|	    << " oe " << PIN_OE
		|	    << " d "
		|'''))

        for sig in self.iter_signals("D", self.width):
            file.write("\t    << %s\n" % sig)

        file.write(self.substitute('''
		|	    << " | " << std::hex << state->data
		|	);
		|
		|	if (state->job > 0) {
		|		uint64_t tmp = state->data;
		|'''))

        for i in self.write_bus_val("Q", self.width, "tmp"):
            file.write('\t\t' + i + '\n')

        file.write(self.substitute('''
		|		state->job = 0;
		|	} else if (state->job == -1) {
		|'''))

        for i in self.write_bus_z("Q", self.width):
            file.write('\t\t' + i + '\n')

        file.write(self.substitute('''
		|		state->job = -2;
		|	}
		|
		|	if (PIN_CLK.posedge()) {
		|		uint64_t tmp = 0;
		|'''))

        for i in self.read_bus_value("tmp", "D", self.width):
            file.write("\t\t" + i + "\n")

        file.write(self.substitute('''
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
		|}
		|'''))

    def hookup(self, file):
        ''' ... '''
        self.hookup_bus(file, "D", self.width)
        self.hookup_bus(file, "Q", self.width)
        self.hookup_pin(file, "PIN_OE", self.nodes["OE"])
        self.hookup_pin(file, "PIN_CLK", self.nodes["CLK"])
