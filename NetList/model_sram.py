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
   Model the SRAM chips
   =====================
'''

from component import ModelComponent

class ModelSRAM(ModelComponent):

    ''' Model various SRAM chips '''

    bus_spec = {}

    def configure(self):
        if self.partname == "2167":
            self.bus_spec["A"] = (0, 13, "sc_in", True, False)
        else:
            print("SRAM", self.partname, ", ".join(self.nodes.keys()))

    def make_clsname(self):
        super().make_clsname()
        if not self.nodes["CS"].net.is_pd():
            self.clsname += "Z"

    def write_code_cc_state_extra(self, file):
        file.write('\tbool ram[16384];\n')

    def write_code_hh_signals(self, file):
        if not self.nodes["CS"].net.is_pd():
            file.write('\tsc_in <sc_logic>\tPIN_CS;\n')
        file.write('\tsc_in <sc_logic>\tPIN_WE;\n')
        file.write('\tsc_in <sc_logic>\tPIN_D;\n')
        file.write('\tsc_out <sc_logic>\tPIN_Q;\n')

    def write_code_cc_sensitive(self, file):
        if not self.nodes["CS"].net.is_pd():
            file.write("\n\t    << PIN_CS")
        file.write("\n\t    << PIN_WE")
        self.write_sensitive_bus(file, "A")

    def write_code_cc(self, file):
        self.write_code_cc_init(file)

        file.write(self.substitute('''
		|void
		|SCM_MMM :: doit(void)
		|{
		|	state->ctx.activations++;
		|
		|	uint64_t a_val = 0;
		|'''))

        for i in self.read_bus_value("a_val", "A"):
            file.write("\t" + i + "\n")

        if not self.nodes["CS"].net.is_pd():
            file.write(self.substitute('''
		|
		|	if (!IS_L(PIN_CS)) {
		|		PIN_Q = sc_logic_Z;
		|		next_trigger(PIN_CS.negedge_event());
		|		TRACE(" Z ");
		|		return;
		|	}
		|'''))

        file.write(self.substitute('''
		|	if (IS_L(PIN_WE))
		|		state->ram[a_val] = IS_H(PIN_D);
		|	PIN_Q = AS(state->ram[a_val]);
		|
		|	TRACE(
		|'''))

        if not self.nodes["CS"].net.is_pd():
            file.write(self.substitute('''
		|	    << " cs " << PIN_CS
		|'''))

        file.write(self.substitute('''
		|	    << " we " << PIN_WE
		|	    << " a "
		|'''))

        for sig in self.iter_signals("A"):
            file.write("\t    << %s\n" % sig)

        file.write(self.substitute('''
		|	    << " | "
		|	    << state->ram[a_val]
		|	);
		|}
		|'''))

    def hookup(self, file):
        ''' ... '''
        self.hookup_bus(file, "A")
        self.hookup_pin(file, "PIN_D", self.nodes["D"])
        self.hookup_pin(file, "PIN_Q", self.nodes["Q"])
        self.hookup_pin(file, "PIN_WE", self.nodes["WE"])
        if not self.nodes["CS"].net.is_pd():
            self.hookup_pin(file, "PIN_CS", self.nodes["CS"])
