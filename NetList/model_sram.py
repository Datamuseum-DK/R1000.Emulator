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

    def configure(self):
        self.bus_spec = {}
        if self.partname == "2167":
            self.alines = 14
            self.width = 1
            self.sepdq = True
        elif self.partname == "2149":
            self.alines = 10
            self.locations = 1<<10
            self.width = 4
            self.sepdq = False
        else:
            print("SRAM", self.partname, ", ".join(self.nodes.keys()))
            assert False
        self.bus_spec["A"] = (0, self.alines - 1, "sc_in", True, False)
        if self.sepdq:
            self.bus_spec["D"] = (0, self.width - 1, "sc_in", True, False)
            self.bus_spec["Q"] = (0, self.width - 1, "sc_out", True, True)
        else:
            self.bus_spec["D"] = (0, self.width - 1, "sc_inout", True, True)
        if self.width <= 8:
            self.dtype = "uint8_t"

        if not self.sepdq:
            nnodes = {}
            for name, node in self.nodes.items():
                if name[:2] == "DQ":
                    node.pin.name = "D" + name[2:]
                nnodes[node.pin.name] = node
            self.nodes = nnodes

    def make_clsname(self):
        super().make_clsname()
        if not self.nodes["CS"].net.is_pd():
            self.clsname += "Z"

    def hookup_model(self, file):
        ''' ... '''
        self.hookup_bus(file, "A")
        self.hookup_bus(file, "D")
        if self.sepdq:
            self.hookup_bus(file, "Q")
        self.hookup_pin(file, "PIN_WE", self.nodes["WE"])
        if not self.nodes["CS"].net.is_pd():
            self.hookup_pin(file, "PIN_CS", self.nodes["CS"])

    def write_code_hh_signals(self, file):
        if not self.nodes["CS"].net.is_pd():
            file.write('\tsc_in <sc_logic>\tPIN_CS;\n')
        file.write('\tsc_in <sc_logic>\tPIN_WE;\n')

    def write_code_cc_state_extra(self, file):
        file.write('\t%s ram[1<<%d];\n' % (self.dtype, self.alines))
        if not self.sepdq:
            file.write('\tconst char *what;\n')
            file.write('\tuint64_t last;\n')

    def write_code_cc_sensitive(self, file):
        if not self.nodes["CS"].net.is_pd():
            file.write("\n\t    << PIN_CS")
        file.write("\n\t    << PIN_WE")
        self.write_sensitive_bus(file, "A")
        if self.sepdq:
            self.write_sensitive_bus(file, "D")

    def write_code_cc(self, file):

        self.write_code_cc_init(file)

        if not self.sepdq:
            if not self.nodes["CS"].net.is_pd():
                file.write(self.substitute('''
		|static const char *ZZZING = "z";
		|'''))

            file.write(self.substitute('''
		|static const char *READING = "r";
		|static const char *WRITING = "w";
		|
		|'''))

        file.write(self.substitute('''
		|void
		|SCM_MMM :: doit(void)
		|{
		|	state->ctx.activations++;
		|'''))

        if self.sepdq:
            self.write_code_sepdq(file)
        else:
            self.write_code_dq(file)

        file.write(self.substitute('''
		|
		|	TRACE(
		|'''))

        if not self.sepdq:
            file.write(self.substitute('''
		|	    << state->what
		|'''))

        if not self.nodes["CS"].net.is_pd():
            file.write(self.substitute('''
		|	    << " cs " << PIN_CS
		|'''))

        file.write(self.substitute('''
		|	    << " we " << PIN_WE
		|	    << " a " << std::hex << a_val
		|	    << " d " << std::hex << d_val
		|'''))

        if False:
            for sig in self.iter_signals("A"):
                if sig[0] == "B":
                    file.write('\t    << "b" << %s\n' % sig)
                else:
                    file.write('\t    << "s" << %s\n' % sig)

        file.write(self.substitute('''
		|	    << " | "
		|	    << (uint64_t)(state->ram[a_val])
		|	);
		|}
		|'''))

    def write_code_sepdq(self, file):
        ''' Write code for separate D & Q pin devices '''

        if not self.nodes["CS"].net.is_pd():
            file.write(self.substitute('''
		|
		|	if (!IS_L(PIN_CS)) {
		|'''))
            for i in self.write_bus_z("Q"):
                file.write('\t\t' + i + '\n')

            file.write(self.substitute('''
		|		next_trigger(PIN_CS.negedge_event());
		|		TRACE(" Z ");
		|		return;
		|	}
		|'''))

        file.write(self.substitute('''
		|
		|	uint64_t a_val = 0;
		|	uint64_t d_val = 0;
		|'''))

        for i in self.read_bus_value("a_val", "A"):
            file.write("\t" + i + "\n")

        file.write(self.substitute('''
		|	d_val = state->ram[a_val];
		|
		|	if (IS_L(PIN_WE)) {
		|'''))

        for i in self.read_bus_value("d_val", "D"):
            file.write("\t\t" + i + "\n")

        file.write(self.substitute('''
		|		state->ram[a_val] = d_val;
		|	}
		|
		|'''))

        for i in self.write_bus_val("Q", "d_val"):
            file.write("\t" + i + "\n")

        file.write(self.substitute('''
		|'''))

    def write_code_dq(self, file):
        ''' Write code for common DQ pin devices '''

        file.write(self.substitute('''
		|
		|	uint64_t a_val = 0;
		|	uint64_t d_val = 0;
		|
		|	if (state->what == NULL)
		|		state->what = READING;
		|'''))

        for i in self.read_bus_value("a_val", "A"):
            file.write("\t" + i + "\n")

        if not self.nodes["CS"].net.is_pd():
            file.write(self.substitute('''
		|
		|	if (IS_H(PIN_CS)) {
		|		if (state->what == READING) {
		|'''))

            for i in self.write_bus_z("D"):
                file.write('\t\t\t' + i + '\n')

            file.write(self.substitute('''
		|		} else if (state->what == WRITING) {
		|'''))

            for i in self.read_bus_value("d_val", "D"):
                file.write("\t\t\t" + i + "\n")

            file.write(self.substitute('''
		|			state->ram[a_val] = d_val;
		|		}
		|		next_trigger(PIN_CS.negedge_event());
		|		state->what = ZZZING;
		|	} else '''))
        else:
            file.write("\n\t")

        file.write(self.substitute('''if (IS_L(PIN_WE)) {
		|		if (state->what == READING) {
		|'''))

        for i in self.write_bus_z("D"):
            file.write('\t\t\t' + i + '\n')

        file.write(self.substitute('''
		|		}
		|
		|'''))

        for i in self.read_bus_value("d_val", "D"):
            file.write("\t\t" + i + "\n")

        file.write(self.substitute('''
		|		state->ram[a_val] = d_val;
		|		state->what = WRITING;
		|	} else {
		|		if (state->what == WRITING) {
		|'''))

        for i in self.read_bus_value("d_val", "D"):
            file.write("\t\t\t" + i + "\n")

        file.write(self.substitute('''
		|			state->ram[a_val] = d_val;
		|			state->what = WRITING;
		|		}
		|		d_val = state->ram[a_val];
		|		if (state->what != READING || d_val != state->last) {
		|'''))

        for i in self.write_bus_val("D", "d_val"):
            file.write("\t\t\t" + i + "\n")

        file.write(self.substitute('''
		|		state->last = d_val;
		|		}
		|		state->what = READING;
		|	}
		|'''))
