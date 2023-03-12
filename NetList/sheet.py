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
        self.mod_type = "mod_" + self.mod_name
        self.components = {}
        self.local_nets = []

        self.scm = self.board.sc_mod(self.mod_name)
        self.scm.add_subst("«ttt»", self.mod_type)

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

    def produce_pub_hh(self, scm):
        ''' ... '''
        scm.fmt('''
		|struct mod_planes;
		|struct mod_«bbb»_globals;
		|struct «mmm»;
		|
		|struct «ttt» *make_«ttt»(
		|    sc_module_name name,
		|    mod_planes &planes,
		|    mod_«bbb»_globals &«bbb»_globals
		|);
		|''')

    def produce_hh(self, scm):
        ''' ... '''
        incls = set()
        for comp in self.components.values():
            for incl in comp.part.yield_includes(comp):
                if incl:
                    incls.add(incl)

        for incl in sorted(incls):
            scm.include(incl)

        scm.fmt('''
		|
		|SC_MODULE(«ttt»)\n
		|{
		|''')

        for net in sorted(self.local_nets):
            if not net.is_supply:
                net.write_decl(scm)

        scm.write("\n")

        for comp in sorted(self.components.values()):
            comp.part.instance(scm, comp)

        scm.fmt('''
		|
		|	«ttt»(
		|	    sc_module_name name,
		|	    mod_planes &planes,
		|	    mod_«bbb»_globals &«bbb»_globals
		|	);
		|};
		|''')

    def produce_cc(self, scm):
        ''' ... '''
        scm.write("#include <systemc.h>\n")
        scm.include(self.board.cpu.planes_hh)
        scm.include(self.board.scm_globals.sf_hh)
        scm.include(self.scm.sf_hh)
        scm.include(self.scm.sf_pub)
        scm.fmt('''
		|
		|struct «ttt» *make_«ttt»(
		|    sc_module_name name,
		|    mod_planes &planes,
		|    mod_«bbb»_globals &«bbb»_globals
		|)
		|{
		|	return new «ttt»(name, planes, «bbb»_globals);
		|}
		|
		|«ttt» :: «ttt»(
		|    sc_module_name name,
		|    mod_planes &planes,
		|    mod_«bbb»_globals &«bbb»_globals
		|) :
		|''')
        scm.write("\tsc_module(name)")
        for net in sorted(self.local_nets):
            if not net.is_supply:
                net.write_init(scm)
        for comp in sorted(self.components.values()):
            comp.part.initialize(scm, comp)
        scm.write("\n{\n")
        for comp in sorted(self.components.values()):
            comp.part.hookup_comp(scm, comp)
        scm.write("}\n")

    def produce(self):
        ''' ... '''
        self.produce_pub_hh(self.scm.sf_pub)
        self.produce_hh(self.scm.sf_hh)
        self.produce_cc(self.scm.sf_cc)
        self.scm.commit()

class SheetSexp(Sheet):
    ''' ... '''
