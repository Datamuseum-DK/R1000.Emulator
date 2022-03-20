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

    busable = True

    def __init__(self, board, sexp):
        self.board = board
        self.sexp = sexp
        self.ref = sexp[0][0].name
        self.value = sexp.find_first("value")[0].name
        self.board.components[self.ref] = self
        self.nodes = {}
        self.busses = []
        self.model = None

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
        text = "\t%s.%s(" % (self.name + suf, pin_no)
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
            node = self.nodes[pin.name]
            if not node.net.bus:
                self.hookup_pin(file, "pin" + pin.num, node, cmt=str(pin))

    def is_bus_ok(self, _bus):
        ''' ... '''
        return False

    def commit_bus(self, bus):
        ''' ... '''
        self.busses.append(bus)

    def do_includes(self):
        ''' ... '''

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


class ModelComponent(Component):
    ''' ModelComponents write their own SystemC implementation source code '''

    busable = True

    bus_spec = {}

    def __init__(self, board, sexp):
        super().__init__(board, sexp)
        self.clsname = None
        self.lcname = None
        self.ucname = None
        self.modname = None

        self.bus_signals = set()
        for bname, bdata in self.bus_spec.items():
            for n in range(bdata[0], bdata[1] + 1):
                self.bus_signals.add(bname + str(n))

    def is_bus_ok(self, bus):
        ''' Accept or reject a bus, and suggest optimal order '''
        mynodes = list(bus.mynodes(self))
        busses = set()
        for node in mynodes:
            if node.pinfunction not in self.bus_signals:
                return False
            busses.add(node.pinfunction[0])
        if len(busses) > 1:
            return False
        mynodes.sort(key=lambda x: x.pinfunction)
        return mynodes

    def do_includes(self):
        ''' This is where the action starts '''

        self.make_clsname()

        old = self.sheet.board.dynamic_components.get(self.clsname)
        if not old:
            self.create_dynamic()
            self.sheet.board.dynamic_components[self.clsname] = self
        else:
            self.scm = old.scm

        self.sheet.scm.hh.include(self.scm.hh)

    def create_dynamic(self):
        self.scm = self.sheet.board.sc_mod(self.modname)
        self.write_code_hh(self.scm.hh)
        self.write_code_cc(self.scm.cc)
        self.scm.commit()
        self.sheet.board.extra_scms.append(self.scm)

    def write_code_cc(self, _file):
        ''' Write the .cc file '''
        assert False

    def iter_nodes(self, name, width):
        ''' iterate the nodes in a bus'''
        for i in range(width):
            node = self.nodes[(name + "%d") % i]
            if (not node.net.bus) or node.net.bus.nets[0] == node.net:
                yield node

    def iter_signals(self, name, width):
        ''' iterate the signals in a bus '''
        for pin, node in enumerate(self.iter_nodes(name, width)):
            if node.net.bus:
                yield "BUS_%s%d" % (name, pin)
            else:
                yield "PIN_%s%d" % (name, pin)

    def make_clsname(self):
        ''' Nail down the class name and see of this model is sharable '''
        signature = []
        nodes = list(self.nodes.values())
        nodes.sort(key=lambda x: x.pinfunction)
        for node in nodes:
            bus = node.net.bus
            if not bus and signature:
                prev = signature[-1]
                if prev[0] == 'P':
                    signature[-1] = ('P', 1 + prev[1])
                else:
                    signature.append(('P', 1))
            elif not bus:
                signature.append(('P', 1))
            elif bus.nets[0] == node.net:
                signature.append(('B', 1, bus))
            elif signature:
                prev = signature[-1]
                if prev[0] == 'B' and prev[2] == bus:
                    signature[-1] = ('B', 1 + prev[1], bus)
                else:
                    signature.append(('b', 1, bus))
        text = ''
        for i in signature:
            if i[1] > 1:
                text += i[0] + str(i[1])
            else:
                text += i[0]
        # print("Signature", self.partname, text)
        if 'b' in text:
            # unordered bus(ses), invent a sharable signature if relevant
            self.clsname = self.name
        else:
            self.clsname = self.partname.upper() + "_" + text
        self.lcname = self.clsname.lower()
        self.ucname = self.clsname.upper()
        self.modname = self.sheet.board.name + "_" + self.ucname

    def substitute(self, text):
        ''' Substitute things into C-source text '''
        if text[0] == '\n':
            text = text[1:]
        for find, replace in (
            ("\t\t|", ""),
            ("ccc", self.partname.lower()),
            ("CCC", self.name),
            ("VVV", self.value),
            ("lll", self.lcname),
            ("UUU", self.ucname),
            ("MMM", self.modname),
        ):
            text = text.replace(find, replace)
        return text

    def write_bus_signals(self, file, typ, prefix, width):
        ''' Write the signals for a bus '''
        for pin, node in enumerate(self.iter_nodes(prefix, width)):
            bus = node.net.bus
            if not bus:
                file.write('\t%s <sc_logic>\tPIN_%s%d;\n' % (typ, prefix, pin))
            else:
                file.write('\t%s_rv<%d>\t\tBUS_%s%d;\n' % (typ, len(bus), prefix, pin))

    def write_code_hh(self, file):
        ''' Write the .hh file '''

        file.write(self.substitute('''
		|
		|#ifndef R1000_MMM
		|#define R1000_MMM
		|
		|struct scm_lll_state;
		|
		|SC_MODULE(SCM_MMM)
		|{
		|'''))

        self.write_code_hh_signals(file)
        for bname, bdata in self.bus_spec.items():
            assert bdata[0] == 0
            self.write_bus_signals(file, bdata[2], bname, 1 + bdata[1] - bdata[0])

        file.write(self.substitute('''
		|
		|	SC_HAS_PROCESS(SCM_MMM);
		|
		|	SCM_MMM(sc_module_name nm, const char *arg);
		|
		|	private:
		|
		|	struct scm_lll_state *state;
		|	void doit(void);
		|'''))

        self.write_code_hh_extra_private(file)

        file.write(self.substitute('''
		|};
		|
		|#endif /* R1000_MMM */
		|'''))

    def write_code_hh_signals(self, _file):
        ''' write the list of SC signals '''
        assert False

    def write_code_cc_init(self, file):
        ''' Write the top of the .cc file, including the initializer '''

        file.write(self.substitute('''
		|
		|#include <systemc.h>
		|#include "Chassis/r1000sc.h"
		|#include "Infra/context.h"
		|
		|'''))

        self.scm.cc.include(self.scm.hh)

        file.write(self.substitute('''
		|
		|struct scm_lll_state {
		|	struct ctx ctx;
		|	uint64_t data;
		|	int job;
		|'''))

        self.write_code_cc_state_extra(file)

        file.write(self.substitute('''
		|};
		|
		|SCM_MMM :: SCM_MMM(sc_module_name nm, const char *arg) : sc_module(nm)
		|{
		|	state = (struct scm_lll_state *)
		|	    CTX_Get("ccc", this->name(), sizeof *state);
		|	should_i_trace(this->name(), &state->ctx.do_trace);
		|'''))

        self.write_code_cc_init_extra(file)

        file.write(self.substitute('''
		|
		|	SC_METHOD(doit);
		|	sensitive'''))

        self.write_code_cc_sensitive(file)

        file.write(";\n")
        file.write('}\n\n')

    def write_code_cc_sensitive(self, _file):
        ''' Write the sensitivity list '''
        assert False

    def write_code_cc_init_extra(self, file):
        ''' Used to add extra initialization '''

    def write_code_cc_state_extra(self, file):
        ''' Used to add variables to state structure '''

    def write_code_hh_extra_private(self, file):
        ''' Used to add mode private fields '''

    def write_sensitive_bus(self, file, prefix, width=0):
        ''' Write sensitivey lines for a bus '''
        if not width:
            blow, bhigh, _bdir = self.bus_spec[prefix]
            assert blow == 0
            width = 1 + bhigh - blow
        for bit, node in enumerate(self.iter_nodes(prefix, width)):
            if node.net.bus:
                file.write("\n\t    << BUS_%s%d" % (prefix, bit))
            else:
                file.write("\n\t    << PIN_%s%d" % (prefix, bit))

    def read_bus_value(self, var, prefix, width):
        ''' Read value of bus '''
        for pin, node in enumerate(self.iter_nodes(prefix, width)):
            assert node.pinfunction[:len(prefix)] == prefix
            bit = width - int(node.pinfunction[len(prefix):]) - 1
            bus = node.net.bus
            if not bus:
                yield 'if (IS_H(PIN_%s%d)) %s |= (1ULL << %d);' % (prefix, pin, var, bit)
                continue
            yield '{'
            bus.tmpname = "bustmp%d" % pin
            yield '\tsc_lv<%d> %s = BUS_%s%d.read();' % (len(bus), bus.tmpname, prefix, pin)
            for idx, bnode in enumerate(bus.mynodes(self)):
                assert bnode.pinfunction[:len(prefix)] == prefix
                bit = width - int(bnode.pinfunction[len(prefix):]) - 1
                bidx = len(bus) - idx - 1
                yield '\tif (IS_H(%s[%d])) %s |= (1ULL << %d);' % (bus.tmpname, bidx, var, bit)
            yield '}'

    def write_bus_z(self, prefix, width):
        ''' Set bus to sc_logic_Z '''
        for pin, node in enumerate(self.iter_nodes(prefix, width)):
            if not node.net.bus:
                yield 'PIN_%s%d = sc_logic_Z;' % (prefix, pin)
            else:
                yield 'BUS_%s%d.write("%s");' % (prefix, pin, 'Z' * width)

    def write_bus_val(self, prefix, width, var):
        ''' Write a value to a bus '''
        for pin, node in enumerate(self.iter_nodes(prefix, width)):
            assert node.pinfunction[:len(prefix)] == prefix
            bit = width - int(node.pinfunction[len(prefix):]) - 1
            bus = node.net.bus

            if not bus:
                yield 'PIN_%s%d = AS(%s & (1ULL << %d));' % (prefix, pin, var, bit)
                continue
            if len(bus) == width:
                good = True
                for idx, bnode in enumerate(bus.mynodes(self)):
                    if bnode.pinfunction != prefix + "%d" % idx:
                        good = False
                        break
                if good:
                    yield 'BUS_%s%d.write(%s);' % (prefix, pin, var)
                    continue
            bus.tmpname = "bustmp%d" % pin
            yield '{'
            yield '\tsc_lv<%d> %s;' % (len(bus), bus.tmpname)
            for idx, bnode in enumerate(bus.mynodes(self)):
                assert bnode.pinfunction[:len(prefix)] == prefix
                bit = width - int(bnode.pinfunction[len(prefix):]) - 1
                bidx = len(bus) - idx - 1
                yield '\t%s[%d] = AS(%s & (1ULL << %d));' % (bus.tmpname, bidx, var, bit)
            yield '\tBUS_%s%d.write(%s);' % (prefix, pin, bus.tmpname)
            yield '}'

    def instance(self, file):
        ''' Write the instance for the .hh file '''
        file.write(self.substitute('\tSCM_MMM CCC;\n'))

    def initialize(self, file):
        ''' Initialize the instance in the .cc file '''
        file.write(self.substitute(',\n\tCCC("CCC", "VVV")'))

    def hookup_bus(self, file, prefix, width):
        ''' Hook this bus into the SC simulation '''
        for pin, node in enumerate(self.iter_nodes(prefix, width)):
            bus = node.net.bus
            if not bus:
                file.write('\t%s.PIN_%s%d(%s);\n' % (self.name, prefix, pin, node.net.cname))
            else:
                file.write('\t%s.BUS_%s%d(%s);\n' % (self.name, prefix, pin, bus.busname))
