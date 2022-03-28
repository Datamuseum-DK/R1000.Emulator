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
   Parts of which components are instances
   =======================================
'''

from pin import PinSexp

class Part():

    ''' A `part` is a type of `component` '''

    def __init__(self, partname):
        self.name = partname
        self.pins = {}
        self.includes = []
        self.ignore = False

        self.clsname = "SCM_" + self.name.upper()

    def __str__(self):
        return "_".join(("Part", self.name))

    def add_pin(self, pin):
        ''' ... '''
        self.pins[pin.ident] = pin

    def configure(self, _board, _comp):
        ''' The 'nets' are ready '''

    def yield_includes(self, _comp):
        yield from self.includes

    def assign(self, _comp):
        ''' When assigned to component '''

    def instance(self, file, comp):
        file.write('\t' + self.clsname + " " + comp.name + ";\n")

    def initialize(self, file, comp):
        file.write(",\n\t" + comp.name + '("' + comp.name + '", "' + comp.value + '")')

    def hookup_comp(self, file, comp):
        if self.ignore:
            return
        file.write("\n\n\t// %s" % " ".join((comp.ref, comp.name, comp.location, comp.partname)))
        if comp.partname != comp.part.name:
            file.write(" (%s)" % comp.part.name)
        file.write("\n")
        self.hookup(file, comp)

    def hookup(self, _file, _comp):
        return

class NoPart(Part):
    ''' Non-Instantiated Part ie: PU, PD, GF, GB '''

    def __init__(self):
        super().__init__("<nopart>")
        self.ignore = True

    def instance(self, _file, _comp):
        return

    def initialize(self, _file, _comp):
        return

    def hookup(self, _file, _comp):
        return

class LibPartSexp(Part):

    ''' Create `part` from netlist-sexp '''

    def __init__(self, board, sexp):
        super().__init__(
            partname = sexp.find_first("part")[0].name
        )
        for pinsexp in sexp.find("pins.pin"):
            self.add_pin(PinSexp(pinsexp))

        self.includes.append('Components/' + self.name + '.hh')
        board.add_part(self.name, self)
        board.add_part(self.name + "_O", self)

    def hookup_pin(self, file, comp, pin_no, node, cmt="", suf=""):
        ''' Text formatting for hooking up a single pin '''
        text = "\t%s.%s(" % (comp.name + suf, pin_no)
        text += node.net.cname
        text += ");"
        if cmt:
            while len(text.expandtabs()) < 64:
                text += "\t"
            text += "// " + cmt
        file.write(text + "\n")

    def hookup(self, file, comp):
        ''' Emit the SystemC code to hook this component up '''
        for pin in sorted(self.pins.values()):
            node = comp.nodes[pin.name]
            if not node.net.bus:
                self.hookup_pin(file, comp, "pin" + pin.ident, node, cmt=str(pin))
