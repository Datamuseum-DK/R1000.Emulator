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

class ModelF181(ModelComponent):
    ''' ... '''

    bus_spec = {
        "A": (0, 3, "sc_in", True, False),
        "B": (0, 3, "sc_in", True, False),
        "S": (0, 3, "sc_in", True, False),
        "Y": (0, 3, "sc_out", True, True),
    }

    signals_in = {
        "CI": ("C1", 13),
        "M": ("M", 12),
    }
    signals_out = {
        "P": ("P", 2),
        "G": ("Q", 0),
        "EQ": ("A=B", 3),
        "CO": ("C0", 1),
    }

    def hookup_model(self, file):
        ''' ... '''
        self.hookup_bus(file, "A")
        self.hookup_bus(file, "B")
        self.hookup_bus(file, "S")
        self.hookup_bus(file, "Y")
        for sig, spec in self.signals_in.items():
            self.hookup_pin(file, "PIN_" + sig, self.nodes[spec[0]])
        for sig, spec in self.signals_out.items():
            self.hookup_pin(file, "PIN_" + sig, self.nodes[spec[0]])

    def write_code_hh_signals(self, file):
        for sig, spec in self.signals_in.items():
            file.write('\tsc_in <sc_logic>\tPIN_%s;\n' % sig)
        for sig, spec in self.signals_out.items():
            file.write('\tsc_out <sc_logic>\tPIN_%s;\n' % sig)

    def write_code_cc_sensitive(self, file):
        for sig, spec in self.signals_in.items():
            file.write('\n\t    << PIN_' + sig)
        self.write_sensitive_bus(file, "A")
        self.write_sensitive_bus(file, "B")
        self.write_sensitive_bus(file, "S")

    def write_code_cc(self, file):
        self.write_code_cc_init(file)

        file.write(self.substitute('''
		|static const uint8_t lut181[16384] = {
		|#include "Components/F181_tbl.h"
		|};
		|
		|void
		|SCM_MMM :: doit(void)
		|{
		|	state->ctx.activations++;
		|
		|	TRACE(
		|	    << " m " << PIN_M
		|	    << " s "
		|'''))

        for sig in self.iter_signals("S"):
            file.write("\t    << %s\n" % sig)

        file.write(self.substitute('''
		|	    << " ci " << PIN_CI
		|	    << " a "
		|'''))

        for sig in self.iter_signals("A"):
            file.write("\t    << %s\n" % sig)

        file.write(self.substitute('''
		|	    << " b "
		|'''))

        for sig in self.iter_signals("B"):
            file.write("\t    << %s\n" % sig)

        file.write(self.substitute('''
		|	);
		|
		|	unsigned idx = 0, tmp;

		|'''))

        for i in self.read_bus_value("tmp", "A"):
            file.write("\t" + i + "\n")

        file.write(self.substitute('''
		|
		|	idx |= tmp << 8;
		|'''))

        for i in self.read_bus_value("tmp", "B"):
            file.write("\t" + i + "\n")

        file.write(self.substitute('''
		|
		|	idx |= tmp << 4;
		|'''))

        for i in self.read_bus_value("tmp", "S"):
            file.write("\t" + i + "\n")

        file.write(self.substitute('''
		|
		|	idx |= tmp;
		|	if (IS_H(PIN_CI)) idx |= (1 << 13);
		|	if (IS_H(PIN_M)) idx |= (1 << 12);
		|
		|	unsigned val = lut181[idx];
		|
		|	tmp = val >> 4;
		|'''))

        for i in self.write_bus_val("Y", "tmp"):
            file.write("\t" + i + "\n")

        file.write(self.substitute('''
		|
		|	if (val & 0x08)
		|		PIN_EQ = sc_logic_Z;
		|	else
		|		PIN_EQ = sc_logic_0;
		|	PIN_P = AS(val & 0x04);
		|	PIN_CO = AS(val & 0x02);
		|	PIN_G = AS(val & 0x01);
		|}
		|'''))
