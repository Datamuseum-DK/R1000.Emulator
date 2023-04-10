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
   Turn kicad netlist files into SystemC source code
   =================================================
'''

class Sheet():
    ''' A `sheet` from the netlist file '''
    def __init__(self, board, sexp):
        self.board = board
        self.sexp = sexp
        name = sexp.find_first("name")[0].name
        self.page = self.board.pagename_to_sheet(name)
        # print("Sheet", board.name, self.page)
        self.mod_name = board.lname + "_%02d" % self.page
        self.mod_type = self.mod_name
        self.components = {}
        self.local_nets = []

        self.scm = self.board.sc_mod(self.mod_name)
        self.scm.add_subst("«ttt»", self.mod_type)
        self.scm.add_ctor_arg("struct planes", "planes", is_ptr=True)
        self.scm.add_ctor_arg("struct «bbb»_globals", "«bbb»_globals", is_ptr=True)

    def __str__(self):
        return self.board.name + "_%d" % self.page

    def __lt__(self, other):
        return self.page < other.page

    def add_component(self, comp):
        ''' Add component to sheet '''
        assert comp.ref not in self.components
        self.components[comp.ref] = comp

    def del_component(self, comp):
        ''' Remove component from sheet '''
        assert comp.ref in self.components
        del self.components[comp.ref]

    def produce(self):
        ''' ... '''

        self.scm.include(self.board.cpu.plane.scm.sf_hh)
        self.scm.include(self.board.scm_globals.sf_hh)

        self.scm.emit_pub_hh()

        for net in sorted(self.local_nets):
            if net.is_supply:
                continue
            for sig in net.sc_signals():
                self.scm.add_signal(sig)

        for comp in sorted(self.components.values()):
            self.scm.add_component(comp)

        self.scm.emit_hh()
        self.scm.emit_cc()
        self.scm.commit()

class SheetSexp(Sheet):
    ''' ... '''
