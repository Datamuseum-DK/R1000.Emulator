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
   Model the F521 chips
   =====================
'''

from component import ModelComponent

class ModelF521(ModelComponent):
    ''' ... '''

    bus_spec = {
        "A": (0, 7, "sc_in"),
        "B": (0, 7, "sc_in"),
    }

    def write_code_hh_signals(self, file):
        file.write('\tsc_in <sc_logic>\tPIN_E_;\n')
        file.write('\tsc_out <sc_logic>\tPIN_A_eq_B_;\n')

    def write_code_cc_sensitive(self, file):
        file.write("\n\t    << PIN_E_")
        self.write_sensitive_bus(file, "A")
        self.write_sensitive_bus(file, "B")

    def write_code_cc(self, file):
        self.write_code_cc_init(file)

        file.write(self.substitute('''
		|void
		|SCM_MMM :: doit(void)
		|{
		|	state->ctx.activations++;
		|
		|	TRACE(
		|	    << " e " << PIN_E_
		|	    << " a "
		|'''))

        for sig in self.iter_signals("A", 8):
            file.write("\t    << %s\n" % sig)

        file.write(self.substitute('''
		|	    << " b "
		|'''))

        for sig in self.iter_signals("B", 8):
            file.write("\t    << %s\n" % sig)

        file.write(self.substitute('''
		|	);
		|
		|	if (IS_H(PIN_E_)) {
		|		PIN_A_eq_B_ = sc_logic_1;
		|		next_trigger(PIN_E_.negedge_event());
		|		return;
		|	}
		|
		|	uint64_t a_val = 0;
		|'''))

        for i in self.read_bus_value("a_val", "A", 8):
            file.write("\t" + i + "\n")

        file.write(self.substitute('''
		|
		|	uint64_t b_val=0;
		|'''))

        for i in self.read_bus_value("b_val", "B", 8):
            file.write("\t" + i + "\n")

        file.write(self.substitute('''
		|
		|	PIN_A_eq_B_ = AS(a_val != b_val);
		|}
		|'''))

    def hookup(self, file):
        ''' ... '''
        self.hookup_bus(file, "A", 8)
        self.hookup_bus(file, "B", 8)
        self.hookup_pin(file, "PIN_E_", self.nodes["E"])
        self.hookup_pin(file, "PIN_A_eq_B_", self.nodes["A=B"])
