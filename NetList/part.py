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

import util

class Part():

    ''' A `part` is a type of `component` '''

    def __init__(self, partname):
        self.name = partname
        self.pins = {}
        self.includes = []
        self.ignore = False
        self.uses = []
        self.blame = set()
        self.busable = False
        self.has_private = False

        self.clsname = "SCM_" + self.name.upper()

    def __str__(self):
        return "_".join(("Part", self.name))

    def add_pin(self, pin):
        ''' ... '''
        self.pins[pin.ident] = pin

    def configure(self, _board, _comp):
        ''' The 'nets' are ready '''

    def yield_includes(self, _comp):
        ''' ... '''
        yield from self.includes

    def assign(self, _comp):
        ''' When assigned to component '''

    def instance(self, file, comp):
        ''' ... '''
        self.uses.append(comp)
        file.write('\t' + self.clsname + " " + comp.name + ";\n")

    def initialize(self, file, comp):
        ''' ... '''
        file.write(",\n\t" + comp.name + '("' + comp.name + '", "' + comp.value + '")')

    def hookup_comp(self, file, comp):
        ''' ... '''
        if self.ignore:
            return
        file.write("\n\n\t// %s" % " ".join((comp.ref, comp.name, comp.location, comp.partname)))
        if comp.partname != comp.part.name:
            file.write(" (%s)" % comp.part.name)
        file.write("\n")
        self.hookup(file, comp)

    def hookup(self, _file, _comp):
        ''' ... '''
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
            if not node.netbus:
                self.hookup_pin(file, comp, "pin" + pin.ident, node, cmt=str(pin))

class PartModel(Part):

    ''' Parts with a python model '''

    def __init__(self, partname, factory = None, busable=True):
        super().__init__(partname)
        self.factory = factory
        self.busable = busable

    def assign(self, comp):
        ''' Assigned to component '''

        for node in comp:
            if node.pin.role[:3] == "sc_":
                continue
            if node.pin.role == "bidirectional":
                node.pin.role = "sc_inout_resolved"
            elif node.pin.role == "open_collector":
                node.pin.role = "sc_out <sc_logic>"
            elif node.pin.role == "input":
                node.pin.role = "c_input"
            elif node.pin.role == "output":
                node.pin.role = "c_output"

    def make_signature(self, comp):
        ''' Produce a signature for this hookup '''

        i = []
        for node in comp:
            if node.pin.pinbus is None and node.net.is_pu():
                i.append("U")
            elif node.pin.pinbus is None and node.net.is_pd():
                i.append("D")
            elif node.netbus:
                if node.netbus.nets[0] == node.net and node.net.sc_type == "bool":
                    i.append("I%d" % len(node.netbus))
                elif node.netbus.nets[0] == node.net:
                    i.append("S%d" % len(node.netbus))
            elif node.net.sc_type == "bool":
                i.append("B")
            else:
                i.append("L")
        return util.signature(i)

    def create(self, board, ident):
        return self.factory(board, ident)

    def configure(self, board, comp):
        sig = self.make_signature(comp)
        ident = self.name + "_" + sig
        if ident not in board.cpu.part_catalog:
            board.cpu.add_part(ident, self.create(board, ident))
        comp.part = board.cpu.part_catalog[ident]

class PartFactory(Part):

    ''' Produce parts on demand '''

    def __init__(self, board, ident):
        super().__init__(ident)
        self.board = board
        self.scm = False
        self.comp = None

    def yield_includes(self, comp):
        ''' (This is the first call we get when used '''

        self.comp = comp
        if not self.scm:
            self.build()
        self.comp = None
        yield self.scm.hh.filename

    def build(self):
        ''' Build this component (if/when used) '''

        self.scm = self.board.cpu.sc_mod(self.name)
        self.subs(self.scm.cc)
        self.scm.std_hh(self.pin_iterator())
        self.scm.std_cc(
            extra = self.extra,
            state = self.state,
            init = self.init,
            sensitive = self.sensitive,
            doit = self.doit,
        )
        self.scm.commit()

    def extra(self, file):
        ''' Extra source-code at globale level'''
        for node in self.comp:
            node.pin.netbus = node.netbus
        for bus in self.comp.busses.values():
            bus.write_extra(file, self.comp)

    def private(self):
        ''' Private variables '''
        for _i in range(0):
            yield None, None

    def state(self, _file):
        ''' Extra state variable '''
        return

    def init(self, file):
        ''' Extra initialization '''
        if self.has_private:
            file.write("\tfirst_initialization = true;\n")

    def sensitive(self):
        ''' sensitivity list '''

        for node in self.comp:
            if node.pin.pinbus is None and node.net.is_pu():
                continue
            if node.pin.pinbus is None and node.net.is_pd():
                continue
            if node.pin.role in ("c_input", "sc_inout_resolved",):
                if not node.netbus:
                    yield "PIN_%s" % node.pin.name
                elif node.netbus.nets[0] == node.net:
                    yield "PINB_%s" % node.pin.name

    def subs(self, file):
        ''' Standard substitutions for .fmt() '''
        for node in self.comp:
            if node.netbus:
                continue
            dst = "PIN_%s<=" % node.pin.name
            src = "PIN_%s=>" % node.pin.name
            trc = "PIN_%s?" % node.pin.name
            if node.pin.role == "c_output" and node.net.sc_type == "bool":
                file.subst(dst, "PIN_%s = " % node.pin.name)
                file.subst(trc, "PIN_%s" % node.pin.name)
            elif node.pin.role == "c_output" or "tri_state" in node.pin.role:
                file.subst(src, "IS_H(PIN_%s)" % node.pin.name)
                file.subst(dst, "PIN_%s = AS" % node.pin.name)
                file.subst(trc, "PIN_%s" % node.pin.name)
            elif node.pin.pinbus is None and node.net.is_pd():
                file.subst(src, "false")
                file.subst(trc, '"v"')
            elif node.pin.pinbus is None and node.net.is_pu():
                file.subst(src, "true")
                file.subst(trc, '"^"')
            elif node.net.sc_type == "bool":
                file.subst(src, "PIN_%s" % node.pin.name)
                file.subst(trc, "PIN_%s" % node.pin.name)
            else:
                file.subst(src, "IS_H(PIN_%s)" % node.pin.name)
                file.subst(dst, "PIN_%s = AS" % node.pin.name)
                file.subst(trc, "PIN_%s" % node.pin.name)

    def doit(self, file):
        ''' The meat of the doit() function '''
        if self.has_private:
            file.write("\tif (first_initialization) {\n")
            file.write("\t\tfirst_initialization = false;\n")
            for _decl, init in self.private():
                file.write("\t\t" + init + ";\n")
            file.write("\t}\n")

    def pin_iterator(self):
        ''' SC pin declarations '''

        for node in self.comp:
            if node.netbus:
                if node.netbus.nets[0] == node.net:

                    pbname = "\tPINB_%s;" % node.pin.name

                    if node.net.sc_type == "bool":
                        sctype = " <uint64_t>"
                    else:
                        sctype = "_rv <%d>" % len(node.netbus.nets)

                    if node.pin.role in ("c_output", "tri_state"):
                        yield "sc_out" + sctype + pbname
                    elif node.pin.role == "sc_inout_resolved":
                        yield "sc_inout" + sctype + pbname
                    else:
                        yield "sc_in" + sctype + pbname

            elif node.pin.role[:3] == "sc_":
                yield "%s\tPIN_%s;" % (node.pin.role, node.pin.name)
            elif node.pin.role == "c_output" and node.net.sc_type == "bool":
                yield "sc_out <bool>\t\tPIN_%s;" % node.pin.name
            elif node.pin.role == "c_output":
                yield "sc_out <sc_logic>\tPIN_%s;" % node.pin.name
            elif node.pin.pinbus is None and node.net.is_pu():
                continue
            elif node.pin.pinbus is None and node.net.is_pd():
                continue
            elif node.net.sc_type == "bool":
                yield "sc_in <bool>\t\tPIN_%s;" % node.pin.name
            elif 'tri_state' in node.pin.role:
                yield "sc_out <sc_logic>\tPIN_%s;" % node.pin.name
            else:
                yield "sc_in <sc_logic>\tPIN_%s;" % node.pin.name + "\t// " + str(node.pin.role)
        for decl, _init in self.private():
            self.has_private = True
            yield decl
        if self.has_private:
            yield "bool first_initialization;"

    def hookup(self, file, comp):
        ''' Hook instance into SystemC model '''

        for node in comp:
            if node.pin.pinbus is None and node.net.is_pu():
                continue
            if node.pin.pinbus is None and node.net.is_pd():
                continue
            if not node.netbus:
                file.write("\t%s.PIN_%s(%s);\n" % (comp.name, node.pin.name, node.net.cname))
            elif node.netbus.nets[0] == node.net:
                file.write("\t%s.PINB_%s(%s);\n" % (comp.name, node.pin.name, node.netbus.cname))

    def event_or(self, name, *args):
        ''' Produce a sc_event_or_list for .private() '''
        j = []
        for pin in args:
            if pin[:4] == "BUS_":
                j.append(pin + "_EVENTS()")
                continue
            assert pin[:4] == "PIN_"
            pname = pin[4:].split(".")[0]
            node = self.comp.nodes.get(pname)
            if not node:
                print("Event pin not found:", pname, self.comp)
                assert node
            if node.net.is_const():
                continue
            if "." in pin:
                j.append(pin)
            else:
                j.append(pin + ".default_event()")
        init = name + " = " + " | ".join(j)
        yield "sc_event_or_list\t" + name + ";", init
