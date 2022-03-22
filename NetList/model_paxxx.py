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
   Model the PAxxx chips
   =====================
'''

from component import ModelComponent

class ModelPAxxx(ModelComponent):
    ''' ... '''

    bus_spec = {
        "A": (0, 8, "sc_in", True, False),
        "Y": (0, 7, "sc_out", False, True),
    }

    def is_bus_ok(self, bus):
        retval = super().is_bus_ok(bus)
        if retval and self.nodes["OE"].net.is_pd():
            retval.numeric = True
        return retval

    def make_clsname(self):
        super().make_clsname()
        if not self.nodes["OE"].net.is_pd():
            self.clsname += "Z"

    def hookup_model(self, file):
        ''' ... '''
        self.hookup_bus(file, "A")
        self.hookup_bus(file, "Y")
        if not self.nodes["OE"].net.is_pd():
            self.hookup_pin(file, "PIN_OE_", self.nodes["OE"])

    def write_code_hh_signals(self, file):
        if not self.nodes["OE"].net.is_pd():
            file.write('\tsc_in <sc_logic>\tPIN_OE_;\n')

    def write_code_hh_extra_private(self, file):
        file.write('\tuint8_t prom[512];\n')

    def write_code_cc_sensitive(self, file):
        if not self.nodes["OE"].net.is_pd():
            file.write("\n\t    << PIN_OE_")
        self.write_sensitive_bus(file, "A")

    def write_code_cc_init_extra(self, file):
        file.write('\tload_programmable(this->name(), prom, sizeof prom, arg);\n')

    def write_code_cc(self, file):
        self.write_code_cc_init(file)

        file.write(self.substitute('''
		|void
		|SCM_MMM :: doit(void)
		|{
		|	state->ctx.activations++;
		|
		|	TRACE(
		|'''))

        if not self.nodes["OE"].net.is_pd():
            file.write(self.substitute('''
		|	    << " e " << PIN_OE_
		|'''))

        file.write(self.substitute('''
		|	    << " a "
		|'''))

        for sig in self.iter_signals("A"):
            file.write("\t    << %s\n" % sig)

        file.write(self.substitute('''
		|	);
		|
		|'''))
        if not self.nodes["OE"].net.is_pd():
            file.write(self.substitute('''
		|	if (IS_H(PIN_OE_)) {
		|'''))

            for i in self.write_bus_z("Y"):
                file.write('\t\t' + i + '\n')

            file.write(self.substitute('''
		|		next_trigger(PIN_OE_.negedge_event());
		|		return;
		|	}
		|'''))

        file.write(self.substitute('''
		|
		|	uint64_t a_val = 0;
		|
		|'''))

        for i in self.read_bus_value("a_val", "A"):
            file.write("\t" + i + "\n")

        file.write(self.substitute('''
		|
		|	uint8_t data = prom[a_val];
		|
		|'''))

        for i in self.write_bus_val("Y", "data"):
            file.write('\t' + i + '\n')

        file.write(self.substitute('''
		|}
		|'''))
