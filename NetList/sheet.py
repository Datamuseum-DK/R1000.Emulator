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
        self.board.sheets[self.page] = self
        # print("Sheet", board.name, self.page)
        self.mod_name = board.name.lower() + "_%02d" % self.page
        self.mod_type = "mod_" + self.mod_name
        self.components = {}
        self.local_nets = []

        self.scm = self.board.sc_mod(self.mod_name)

    def __str__(self):
        return self.board.name + "_%d" % self.page

    def substitute(self, text):
        ''' Substitute things into C-source text '''
        for find, replace in (
            ("\t\t|", ""),
            ("mmm", self.mod_name),
            ("ttt", self.mod_type),
        ):
            text = text.replace(find, replace)
        return self.board.substitute(text)

    def produce_pub_hh(self, scm):
        ''' ... '''
        scm.write(self.substitute('''
		|struct mod_planes;
		|struct mod_lll_globals;
		|struct mmm;
		|
		|struct ttt *make_ttt(
		|    sc_module_name name,
		|    mod_planes &planes,
		|    mod_lll_globals &lll_globals
		|);
		|'''))

    def produce_hh(self, scm):
        ''' ... '''
        incls = set()
        for comp in self.components.values():
            for incl in comp.include_files():
                if incl:
                    incls.add(incl)
        for incl in sorted(incls):
            scm.include(incl)
        scm.write("\n")
        scm.write("SC_MODULE(%s)\n" % self.mod_type)
        scm.write("{\n")
        for net in sorted(self.local_nets):
            net.write_decl(scm)
        scm.write("\n")
        for comp in sorted(self.components.values()):
            comp.instance(scm)
        scm.write("\n")
        scm.write(self.substitute('''
		|	ttt(
		|	    sc_module_name name,
		|	    mod_planes &planes,
		|	    mod_lll_globals &lll_globals
		|	);
		|};
		|'''))

    def produce_cc(self, scm):
        ''' ... '''
        scm.write("#include <systemc.h>\n")
        scm.include("Chassis/planes.hh")
        scm.include(self.board.scm_globals.hh)
        scm.include(self.scm.hh)
        scm.include(self.scm.pub)
        scm.write(self.substitute('''
		|
		|struct ttt *make_ttt(
		|    sc_module_name name,
		|    mod_planes &planes,
		|    mod_lll_globals &lll_globals
		|)
		|{
		|	return new ttt(name, planes, lll_globals);
		|}
		|
		|ttt :: ttt(
		|    sc_module_name name,
		|    mod_planes &planes,
		|    mod_lll_globals &lll_globals
		|) :
		|'''))
        scm.write("\tsc_module(name)")
        for net in sorted(self.local_nets):
            net.write_init(scm)
        for comp in sorted(self.components.values()):
            comp.initialize(scm)
        scm.write("\n{\n")
        for comp in sorted(self.components.values()):
            comp.hookup(scm)
        scm.write("}\n")

    def produce(self):
        ''' ... '''
        self.produce_pub_hh(self.scm.pub)
        self.produce_hh(self.scm.hh)
        self.produce_cc(self.scm.cc)
        self.scm.commit()
