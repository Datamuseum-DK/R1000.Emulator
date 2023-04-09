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
   A "board" is the contents of a single KiCad Netlist file (=project)
   ===================================================================
'''

import os

from sexp import SExp

from srcfile import Makefile
from scmod import SystemCModule
from sheet import SheetSexp
from part import LibPartSexp, NoPart
from net import NetSexp
from component import ComponentSexp

class Board():

    ''' Ingest one KiCad netlist file '''

    def __init__(self, cpu, netlist):
        self.cpu = cpu
        self.branch = cpu.branch
        self.sexp = SExp(None)
        self.sexp.parse(open(netlist).read())
        self.find_board_name()
        self.dstdir = os.path.join(
             cpu.workdir,
             self.name.capitalize(),
        )
        os.makedirs(self.dstdir, exist_ok=True)
        self.srcs = []
        self.part_catalog = self.cpu.part_catalog

        self.makefile = Makefile(self.dstdir + "/Makefile.inc")
        self.scm_board = self.sc_mod(self.lname + "_board")
        self.scm_board.add_subst("«bbb»", self.lname)
        self.scm_board.add_subst("«BBB»", self.name)
        self.scm_globals = self.sc_mod(self.lname + "_globals")
        self.scm_globals.add_subst("«bbb»", self.lname)
        self.scm_globals.add_subst("«BBB»", self.name)

        self.scm_board.add_ctor_arg("struct planes", "planes", is_ptr = True)

        self.sheets = {}
        for i in self.sexp.find("design.sheet"):
            sheet = SheetSexp(self, i)
            self.sheets[sheet.page] = sheet

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

    def __repr__(self):
        return self.name

    def __lt__(self, other):
        return self.name < other.name

    def add_part(self, name, part):
        ''' Add a part to our catalog, if not already occupied '''
        self.cpu.add_part(name, part)

    def add_net(self, net):
        ''' ... '''
        self.nets[net.name] = net
        self.cpu.nets[self.name + "." + net.name] = net

    def add_z_code(self, comp, zcode):
        ''' ... '''
        self.cpu.add_z_code(comp, zcode)

    def del_net(self, net):
        ''' ... '''
        del self.nets[net.name]
        del self.cpu.nets[self.name + "." + net.name]

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
        scm = SystemCModule(self.dstdir + "/" + basename, self.makefile)
        scm.add_subst("«bbb»", self.lname)
        return scm

    def produce(self):
        ''' ... '''
        os.makedirs(self.dstdir, exist_ok=True)

        self.scm_board.add_child(self.scm_globals.basename.lower(), self.scm_globals)
        for sheet in self.sheets.values():
            self.scm_board.add_child(sheet.mod_name, sheet.scm)

        self.scm_board.emit_pub_hh()
        self.scm_board.emit_hh()
        self.scm_board.emit_cc()
        self.scm_board.commit()

        for net in sorted(self.nets.values()):
            for sig in net.sc_signals():
                self.scm_globals.add_signal(sig)

        self.scm_globals.emit_pub_hh()
        self.scm_globals.emit_hh()
        self.scm_globals.emit_cc()
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
