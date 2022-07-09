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

import os

from sexp import SExp

from srcfile import SrcFile
from scmod import SC_Mod
from sheet import SheetSexp
from part import LibPartSexp, NoPart
from net import NetSexp
from component import ComponentSexp

from pass_net_config import PassNetConfig
from pass_part_config import PassPartConfig

class Board():
    ''' A netlist file '''
    def __init__(self, cpu, netlist):
        self.cpu = cpu
        self.branch = cpu.branch
        self.sexp = SExp(None)
        self.sexp.parse(open(netlist).read())
        self.find_board_name()
        self.dstdir = self.name.capitalize() + "/" + self.branch
        os.makedirs(self.dstdir, exist_ok=True)
        self.srcs = []
        self.part_catalog = self.cpu.part_catalog

        self.makefile = SrcFile(self.dstdir + "/Makefile.inc")
        self.chf_sheets = SrcFile(self.dstdir + "/" + self.lname + "_sheets.h")
        self.scm_board = self.sc_mod(self.lname + "_board")
        self.scm_board.subst("«bbb»", self.lname)
        self.scm_board.subst("«BBB»", self.name)
        self.scm_globals = self.sc_mod(self.lname + "_globals")
        self.scm_globals.subst("«bbb»", self.lname)
        self.scm_globals.subst("«BBB»", self.name)

        self.sheets = {}
        for i in self.sexp.find("design.sheet"):
            SheetSexp(self, i)

        self.add_part("GB", NoPart())
        self.add_part("GF", NoPart())
        self.add_part("Pull_Up", NoPart())
        self.add_part("Pull_Down", NoPart())
        for libpartsexp in self.sexp.find("libparts.libpart"):
            LibPartSexp(self, libpartsexp)

        self.components = {}
        for compsexp in self.sexp.find("components.comp"):
            ComponentSexp(self, compsexp)

        self.nets = {}
        for netsexp in self.sexp.find("nets.net"):
            NetSexp(self, netsexp)

    def __str__(self):
        return self.name

    def __lt__(self, other):
        return self.name < other.name

    def chew(self):
        PassNetConfig(self)
        PassPartConfig(self)

    def add_part(self, name, part):
        ''' Add a part to our catalog, if not already occupied '''
        self.cpu.add_part(name, part)

    def add_net(self, net):
        ''' ... '''
        self.nets[net.name] = net

    def iter_components(self):
        ''' ... '''
        yield from list(self.components.values())

    def iter_nets(self):
        ''' ... '''
        yield from self.nets.values()

    def get_component(self, name):
        ''' ... '''
        retval = self.components.get(name)
        if not retval:
            raise NameError(name)
        return retval

    def find_board_name(self):
        ''' We dont trust the filename '''
        title = self.sexp.find_first('design.sheet.title_block.title')
        i = title[0].name.split()
        assert i[1] == "Main"
        assert i[0].upper() == i[0]
        self.name = i[0]
        self.lname = i[0].lower()

    def sc_mod(self, basename):
        ''' ... '''
        return SC_Mod(self.dstdir + "/" + basename, self.makefile)

    def produce_sheets_h(self, file):
        ''' ... '''
        file.write("#define " + self.name + "_N_SHEETS %d\n" % len(self.sheets))
        file.write("#define " + self.name + "_SHEETS()")
        for sheet in self.sheets.values():
            file.write(" \\\n\tSHEET(" + self.name)
            file.write(", " + self.name.lower())
            file.write(", %2d" % sheet.page)
            file.write(", %02d" % sheet.page + ")")
        file.write("\n")

    def produce_board_pub_hh(self, scm):
        ''' ... '''
        scm.fmt('''
		|struct mod_planes;
		|struct mod_«bbb»;
		|
		|struct mod_«bbb» *make_mod_«bbb»(sc_module_name name, mod_planes &planes, const char *how);
		|''')

    def produce_board_hh(self, scm):
        ''' ... '''
        scm.include(self.scm_globals.hh)
        scm.include(self.chf_sheets)
        scm.fmt('''
		|
		|#define SHEET(upper, lower, nbr, fmt) struct mod_##lower##_##fmt;
		|«BBB»_SHEETS()
		|#undef SHEET
		|
		|SC_MODULE(mod_«bbb»)
		|{
		|	mod_«bbb»_globals «bbb»_globals;
		|	#define SHEET(upper, lower, nbr, fmt) mod_##lower##_##fmt *lower##_##fmt;
		|	«BBB»_SHEETS()
		|	#undef SHEET
		|
		|	mod_«bbb»(sc_module_name name, mod_planes &planes, const char *how);
		|};
		|''')

    def produce_board_cc(self, scm):
        ''' ... '''
        scm.write("#include <systemc.h>\n")
        scm.include("Chassis/%s/planes.hh" % self.branch)
        scm.include(self.scm_board.hh)
        scm.include(self.scm_board.pub)
        scm.fmt('''
		|
		|''')
        for sheet in self.sheets.values():
            scm.include(sheet.scm.pub)
        scm.fmt('''
		|
		|struct mod_«bbb» *make_mod_«bbb»(
		|    sc_module_name name,
		|    mod_planes &planes,
		|    const char *how
		|)
		|{
		|	return new mod_«bbb»(name, planes, how);
		|}
		|
		|mod_«bbb» :: mod_«bbb»(
		|    sc_module_name name,
		|    mod_planes &planes,
		|    const char *how
		|) :
		|	sc_module(name),
		|	«bbb»_globals("«bbb»_globals")
		|{
		|	if (how == NULL)
		|''')
        scm.write('\t\thow = "%s";\n' % ('+' * len(self.sheets)))
        scm.write('\tassert(strlen(how) == %d);\n' % len(self.sheets))
        # ... we could also use the SHEET macro ...
        for sheet in self.sheets.values():
            scm.write("\tif (*how++ == '+')\n")
            scm.write('\t\t%s = ' % sheet.mod_name)
            scm.write(' make_%s(' % sheet.mod_type)
            scm.write('"%s", planes, %s_globals);\n' % (sheet.mod_name, self.lname))
        scm.write("}\n")

    def produce_globals_pub_hh(self, scm):
        ''' ... '''
        scm.fmt('''
		|struct mod_«bbb»_globals;
		|
		|struct mod_«bbb»_globals *make_mod_«bbb»_globals(sc_module_name name);
		|''')

    def produce_globals_hh(self, scm):
        ''' ... '''
        scm.fmt('''
		|SC_MODULE(mod_«lll»)
		|{
		|''')
        for net in sorted(self.nets.values()):
            if net.is_local or net.is_plane:
                continue
            net.write_decl(scm)
        scm.fmt('''
		|
		|	mod_«lll»(sc_module_name name);
		|};
		|''')

    def produce_globals_cc(self, scm):
        ''' ... '''
        scm.write("#include <systemc.h>\n")
        scm.include(self.scm_globals.hh)
        scm.include(self.scm_globals.pub)
        scm.fmt('''
		|
		|struct mod_«bbb»_globals *make_mod_«bbb»_globals(sc_module_name name)
		|{
		|	return new mod_«bbb»_globals(name);
		|}
		|
		|mod_«bbb»_globals :: mod_«bbb»_globals(sc_module_name name) :
		|''')
        scm.write("\tsc_module(name)")
        for net in sorted(self.nets.values()):
            if net.is_local or net.is_plane:
                continue
            net.write_init(scm)
        scm.write("\n{\n}\n")

    def produce(self):
        ''' ... '''
        os.makedirs(self.dstdir, exist_ok=True)

        self.produce_sheets_h(self.chf_sheets)
        self.chf_sheets.commit()

        self.produce_board_pub_hh(self.scm_board.pub)
        self.produce_board_hh(self.scm_board.hh)
        self.produce_board_cc(self.scm_board.cc)
        self.scm_board.commit()

        self.produce_globals_pub_hh(self.scm_globals.pub)
        self.produce_globals_hh(self.scm_globals.hh)
        self.produce_globals_cc(self.scm_globals.cc)
        self.scm_globals.commit()

        for sheet in self.sheets.values():
            sheet.produce()

        self.makefile.commit()

    def pagename_to_sheet(self, text):
        ''' Convert a sheets name to (our) sheet number '''
        if text == "/":
            return 0
        assert text[:6] == "/Page "
        assert text[-1] == "/"
        return int(text[6:-1], 10)
