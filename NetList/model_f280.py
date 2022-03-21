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
   Model the F280 chips
   =====================
'''

from component import ModelComponent

class ModelF280(ModelComponent):
    ''' ... '''

    bus_spec = {
        "I": (0, 8, "sc_in", True, False),
    }

    def write_code_hh_signals(self, file):
        file.write('\tsc_out <sc_logic>\tPIN_PEV;\n')
        file.write('\tsc_out <sc_logic>\tPIN_POD;\n')

    def write_code_cc_sensitive(self, file):
        self.write_sensitive_bus(file, "I")

    def write_code_cc(self, file):
        self.write_code_cc_init(file)

        file.write(self.substitute('''
		|void
		|SCM_MMM :: doit(void)
		|{
		|	state->ctx.activations++;
		|
		|	uint64_t i_val = 0;
		|'''))

        for i in self.read_bus_value("i_val", "I"):
            file.write("\t" + i + "\n")

        file.write(self.substitute('''
		|
		|	i_val ^= i_val >> 8;
		|	i_val ^= i_val >> 4;
		|	i_val ^= i_val >> 2;
		|	i_val ^= i_val >> 1;
		|	i_val &= 1;
		|	PIN_PEV = AS(!i_val);
		|	PIN_POD = AS(i_val);
		|
		|	TRACE(
		|	    << " i "
		|'''))

        for sig in self.iter_signals("I"):
            file.write("\t    << %s\n" % sig)

        file.write(self.substitute('''
		|	    << " | "
		|	    << i_val
		|	);
		|}
		|'''))

    def hookup(self, file):
        ''' ... '''
        self.hookup_bus(file, "I")
        self.hookup_pin(file, "PIN_PEV", self.nodes["PEV"])
        self.hookup_pin(file, "PIN_POD", self.nodes["POD"])
