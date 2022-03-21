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
   Model the F240 and F244 chips as XBUF4(s) or XBUF8
   ==================================================
'''

from component import ModelComponent

from libpart import LibPart

class ModelF24x(ModelComponent):
    ''' F240/F244 '''

    bus_spec = {
        "I": (0, 7, "sc_in", True, False),
        "Y": (0, 7, "sc_out", False, True),
    }

    def configure(self):
        self.noz = self.nodes["OE0"].net.is_pd() and self.nodes["OE1"].net.is_pd()
        self.same = self.nodes["OE0"].net == self.nodes["OE1"].net

    def is_bus_ok(self, bus):
        self.configure()
        if self.fallback:
            return False
        retval = super().is_bus_ok(bus)
        if retval and retval.output and not self.same:
            return False
        if retval and self.noz:
            retval.numeric = True
        return retval

    def make_clsname(self):
        super().make_clsname()
        self.configure()
        if self.fallback:
            return
        if not self.noz:
            self.clsname += "Z"
        if self.same:
            self.clsname += "W"

    def write_code_hh_signals(self, file):
        if not self.noz:
            file.write('\tsc_in <sc_logic>\tPIN_OE0;\n')
            if not self.same:
                file.write('\tsc_in <sc_logic>\tPIN_OE1;\n')

    def write_code_cc_sensitive(self, file):
        if not self.noz:
            file.write("\n\t    << PIN_OE0")
            if not self.same:
                file.write("\n\t    << PIN_OE1")
        self.write_sensitive_bus(file, "I")

    def hookup(self, file):
        ''' ... '''
        if self.fallback:
            super().hookup(file)
            return
        self.hookup_bus(file, "I")
        self.hookup_bus(file, "Y")
        if not self.noz:
            self.hookup_pin(file, "PIN_OE0", self.nodes["OE0"])
            if not self.same:
                self.hookup_pin(file, "PIN_OE1", self.nodes["OE1"])

    def write_code_cc_state_extra(self, file):
        file.write("\tuint64_t inv;\n")
        if not self.same:
            file.write("\tint job2;\n")

    def write_code_cc_init_extra(self, file):
        file.write("\tstate->job = -1;\n")
        if not self.same:
            file.write("\tstate->job2 = -1;\n")
        if self.partname == "F240":
            file.write("\tstate->inv = 0xff;\n")
        else:
            assert self.partname == "F244"
            file.write("\tstate->inv = 0;\n")

    def write_code_cc(self, file):
        if self.same:
            self.write_code_cc_one(file)
        else:
            self.write_code_cc_two(file)

    def write_code_cc_one(self, file):
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
		|	    << " oe " << PIN_OE0
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
		|		next_trigger(PIN_OE0.negedge_event());
		|	}
		|
		|	if (IS_H(PIN_OE0)) {
		|		if (state->job != -2) {
		|			state->job = -1;
		|			next_trigger(5, SC_NS);
		|		} else {
		|			next_trigger(PIN_OE0.negedge_event());
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

    def write_code_cc_two(self, file):
        self.write_code_cc_init(file)

        file.write(self.substitute('''
		|void
		|SCM_MMM :: doit(void)
		|{
		|	state->ctx.activations++;
		|
		|	TRACE(
		|	    << " job1 " << state->job
		|	    << " job2 " << state->job2
		|'''))

        if not self.noz:
            file.write(self.substitute('''
		|	    << " oe " << PIN_OE0 << PIN_OE1
		|'''))

        for sig in self.iter_signals("I"):
            file.write("\t    << %s\n" % sig)

        file.write(self.substitute('''
		|	);
		|
		|	if (state->job > 0) {
		|		uint64_t tmp = state->data ^ state->inv;
		|		PIN_Y0 = AS(tmp & 0x80);
		|		PIN_Y1 = AS(tmp & 0x40);
		|		PIN_Y2 = AS(tmp & 0x20);
		|		PIN_Y3 = AS(tmp & 0x10);
		|		state->job = 0;
		|	}
		|	if (state->job2 > 0) {
		|		uint64_t tmp = state->data ^ state->inv;
		|		PIN_Y4 = AS(tmp & 0x08);
		|		PIN_Y5 = AS(tmp & 0x04);
		|		PIN_Y6 = AS(tmp & 0x02);
		|		PIN_Y7 = AS(tmp & 0x01);
		|		state->job2 = 0;
		|	}
		|'''))

        if not self.noz:
            file.write(self.substitute('''
		|	if (state->job == -1) {
		|		PIN_Y0 = sc_logic_Z;
		|		PIN_Y1 = sc_logic_Z;
		|		PIN_Y2 = sc_logic_Z;
		|		PIN_Y3 = sc_logic_Z;
		|		state->job = -2;
		|	}
		|	if (state->job2 == -1) {
		|		PIN_Y4 = sc_logic_Z;
		|		PIN_Y5 = sc_logic_Z;
		|		PIN_Y6 = sc_logic_Z;
		|		PIN_Y7 = sc_logic_Z;
		|		state->job2 = -2;
		|	}
		|
		|
		|	if (IS_H(PIN_OE0) && state->job != -2)
		|		state->job = -1;
		|	else if (IS_L(PIN_OE0) && state->job < 0)
		|		state->job = 1;
		|
		|	if (IS_H(PIN_OE1) && state->job2 != -2)
		|		state->job2 = -1;
		|	else if (IS_L(PIN_OE1) && state->job2 < 0)
		|		state->job2 = 1;
		|
		|'''))

        file.write(self.substitute('''
		|
		|	uint64_t i_val = 0;
		|'''))

        for i in self.read_bus_value("i_val", "I"):
            file.write("\t" + i + "\n")

        file.write(self.substitute('''
		|
		|	if (i_val ^ state->data) {
		|		if (IS_L(PIN_OE0)) {
		|			state->data = i_val;
		|			state->job = 1;
		|		}
		|		if (IS_L(PIN_OE1)) {
		|			state->data = i_val;
		|			state->job2 = 1;
		|		}
		|	}
		|
		|	if (state->job == -1 || state->job2 == -1)
		|		next_trigger(5, SC_NS);
		|	else if (state->job == 1 || state->job2 == 1)
		|		next_trigger(5, SC_NS);
		|'''))
        if not self.noz:
            file.write(self.substitute('''
		|	else if (IS_H(PIN_OE0) && IS_H(PIN_OE1))
		|		next_trigger(PIN_OE0.negedge_event() | PIN_OE1.negedge_event());
		|'''))

        file.write(self.substitute('''
		|}
		|'''))
