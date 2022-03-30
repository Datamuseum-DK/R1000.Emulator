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

import model_nand
import model_nor
import model_mux2
# import model_2149
import model_2167
import model_93s48
import model_f51
import model_f74
import model_f86
import model_f138
import model_f139
import model_f148
import model_f151
import model_f153
import model_f169
import model_f174
import model_f175
import model_f181
import model_f182
import model_f194
import model_f280
import model_f283
import model_f381
import model_f521
import model_paxxx
import model_xbuf
import model_xreg

from pass_planes import PassPlanes
from pass_assign_part import PassAssignPart
from pass_net_config import PassNetConfig
from pass_part_config import PassPartConfig

class Board():
    ''' A netlist file '''
    def __init__(self, netlist, branch):
        self.branch = branch
        self.sexp = SExp(None)
        self.sexp.parse(open(netlist).read())
        self.find_board_name()
        print("Board", self.name)
        self.dstdir = self.name.capitalize() + "/" + branch
        os.makedirs(self.dstdir, exist_ok=True)
        self.srcs = []
        self.dynamic_components = {}
        self.extra_scms = []
        self.part_catalog = {}

        if True:
            model_nand.register(self)
            model_nor.register(self)
            model_mux2.register(self)
            # model_2149.register(self)
            model_2167.register(self)
            model_93s48.register(self)
            model_f51.register(self)
            model_f74.register(self)
            model_f86.register(self)
            model_f138.register(self)
            model_f139.register(self)
            model_f148.register(self)
            model_f151.register(self)
            model_f153.register(self)
            model_f169.register(self)
            model_f174.register(self)
            model_f175.register(self)
            model_f181.register(self)
            model_f182.register(self)
            model_f194.register(self)
            model_f280.register(self)
            model_f283.register(self)
            model_f381.register(self)
            model_f521.register(self)
            model_paxxx.register(self)
            model_xbuf.register(self)
            model_xreg.register(self)

        self.chf_sheets = SrcFile(self.dstdir + "/" + self.lname + "_sheets.h")
        self.scm_board = self.sc_mod(self.lname + "_board")
        self.scm_globals = self.sc_mod(self.lname + "_globals")

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
            net = NetSexp(self, netsexp)
            self.nets[net.name] = net

        PassPlanes(self)
        PassAssignPart(self)
        PassNetConfig(self)
        PassPartConfig(self)

    def __str__(self):
        return self.name

    def add_part(self, name, part):
        if name not in self.part_catalog:
            self.part_catalog[name] = part

    def iter_components(self):
        yield from list(self.components.values())

    def iter_nets(self):
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
        return SC_Mod(self.dstdir + "/" + basename)

    def substitute(self, text):
        ''' Substitute things into C-source text '''
        for find, replace in (
            ("\t\t|", ""),
            ("UUU", self.name),
            ("lll", self.lname),
        ):
            text = text.replace(find, replace)
        return text

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
        scm.write(self.substitute('''
		|struct mod_planes;
		|struct mod_lll;
		|
		|struct mod_lll *make_mod_lll(sc_module_name name, mod_planes &planes, const char *how);
		|'''))

    def produce_board_hh(self, scm):
        ''' ... '''
        scm.include(self.scm_globals.hh)
        scm.include(self.chf_sheets)
        scm.write(self.substitute('''
		|
		|#define SHEET(upper, lower, nbr, fmt) struct mod_##lower##_##fmt;
		|UUU_SHEETS()
		|#undef SHEET
		|
		|SC_MODULE(mod_lll)
		|{
		|	mod_lll_globals lll_globals;
		|	#define SHEET(upper, lower, nbr, fmt) mod_##lower##_##fmt *lower##_##fmt;
		|	UUU_SHEETS()
		|	#undef SHEET
		|
		|	mod_lll(sc_module_name name, mod_planes &planes, const char *how);
		|};
		|'''))

    def produce_board_cc(self, scm):
        ''' ... '''
        scm.write("#include <systemc.h>\n")
        scm.include("Chassis/planes.hh")
        scm.include(self.scm_board.hh)
        scm.include(self.scm_board.pub)
        scm.write(self.substitute('''
		|
		|'''))
        for sheet in self.sheets.values():
            scm.include(sheet.scm.pub)
        scm.write(self.substitute('''
		|
		|struct mod_lll *make_mod_lll(
		|    sc_module_name name,
		|    mod_planes &planes,
		|    const char *how
		|)
		|{
		|	return new mod_lll(name, planes, how);
		|}
		|
		|mod_lll :: mod_lll(
		|    sc_module_name name,
		|    mod_planes &planes,
		|    const char *how
		|) :
		|	sc_module(name),
		|	lll_globals("lll_globals")
		|{
		|	if (how == NULL)
		|'''))
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
        scm.write(self.substitute('''
		|struct mod_lll_globals;
		|
		|struct mod_lll_globals *make_mod_lll_globals(sc_module_name name);
		|'''))

    def produce_globals_hh(self, scm):
        ''' ... '''
        scm.write("\nSC_MODULE(mod_%s_globals)\n" % self.lname)
        scm.write("{\n")
        for net in sorted(self.nets.values()):
            if net.is_local or net.is_plane:
                continue
            net.write_decl(scm)
        scm.write(self.substitute('''
		|
		|	mod_lll_globals(sc_module_name name);
		|};
		|'''))

    def produce_globals_cc(self, scm):
        ''' ... '''
        scm.write("#include <systemc.h>\n")
        scm.include(self.scm_globals.hh)
        scm.include(self.scm_globals.pub)
        scm.write(self.substitute('''
		|
		|struct mod_lll_globals *make_mod_lll_globals(sc_module_name name)
		|{
		|	return new mod_lll_globals(name);
		|}
		|
		|mod_lll_globals :: mod_lll_globals(sc_module_name name) :
		|'''))
        scm.write("\tsc_module(name)")
        for net in sorted(self.nets.values()):
            if net.is_local or net.is_plane:
                continue
            net.write_init(scm)
        scm.write("\n{\n}\n")

    def produce_makefile_inc(self, file):
        ''' ... '''
        self.scm_board.makefile(file)
        self.scm_globals.makefile(file)
        for sheet in self.sheets.values():
            sheet.scm.makefile(file)
        for scm in self.extra_scms:
            scm.makefile(file)

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

        chf = SrcFile(self.dstdir + "/Makefile.inc")
        self.produce_makefile_inc(chf)
        chf.commit()

        open(self.dstdir + "/_timestamp", "w").write("\n")

    def pagename_to_sheet(self, text):
        ''' Convert a sheets name to (our) sheet number '''
        if text == "/":
            return 0
        assert text[:6] == "/Page "
        assert text[-1] == "/"
        return int(text[6:-1], 10)
