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

import transit

class Component():
    ''' A `component` from the netlist file '''

    busable = False

    def __init__(self, board, sexp):
        self.board = board
        self.sexp = sexp
        self.ref = sexp[0][0].name
        self.value = sexp.find_first("value")[0].name
        self.board.components[self.ref] = self
        self.nodes = {}

        i = self.sexp.find_first("sheetpath.names")
        self.sheet = board.sheets[board.pagename_to_sheet(i[0].name)]
        self.sheet.components[self.ref] = self

        i = self.sexp.find_first("libsource.part")
        self.partname = i[0].name
        self.is_plane = self.partname in ("GF", "GB")
        if self.is_plane:
            self.ref = self.ref[:2] + "%03d" % int(self.ref[2:], 10)
            self.ref = transit.do_transit(self.board.name, self.ref)
        self.part = self.board.libparts[self.partname]

        self.location = "x99"
        self.name = "X"
        if self.is_plane:
            self.name = self.ref
        for i in self.sexp.find("property"):
            if i[0][0].name == 'Location':
                self.location = i[1][0].name
            elif i[0][0].name == 'Name':
                self.name = i[1][0].name

        self.scm = "SCM_" + self.part.partname.upper()

    def __str__(self):
        return "_".join((str(self.sheet), self.ref, self.partname, self.location, self.name))

    def __lt__(self, other):
        return self.name < other.name

    def add_node(self, node):
        ''' Add a node to this component '''
        self.nodes[node.pinfunction] = node

    def post_parse(self):
        ''' Chance to chew on things between parsing and generation '''

    def include_files(self):
        ''' Register necessary include files '''
        yield self.part.include_file()

    def instance(self, file):
        ''' Emit the local instance of this component '''
        file.write('\t' + self.scm + " " + self.name + ";\n")

    def initialize(self, file):
        ''' Initialize the local instance of this component '''
        file.write(",\n\t" + self.name + '("' + self.name + '", "' + self.value + '")')

    def hookup_pin(self, file, pin_no, node, cmt="", suf=""):
        ''' Text formatting for hooking up a single pin '''
        text = "\t%s.pin%s(" % (self.name + suf, pin_no)
        text += node.net.cname
        text += ");"
        if cmt:
            while len(text.expandtabs()) < 64:
                text += "\t"
            text += "// " + cmt
        file.write(text + "\n")

    def hookup(self, file):
        ''' Emit the SystemC code to hook this component up '''
        file.write("\n\n\t// %s\n" % " ".join((self.ref, self.name, self.location, self.partname)))
        for pin in sorted(self.part.pins.values()):
            self.hookup_pin(file, pin.num, self.nodes[pin.name], cmt=str(pin))

class VirtualComponent(Component):
    ''' Components not instantiated in SystemC '''

    def include_files(self):
        if False:
            yield None

    def instance(self, file):
        return

    def initialize(self, file):
        return

    def hookup(self, file):
        return
