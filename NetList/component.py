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

    def __init__(self, board, compref, compvalue, compsheet, comppart):
        self.board = board
        self.ref = compref
        self.value = compvalue
        self.board.components[self.ref] = self
        self.nodes = {}
        self.busses = []
        self.model = None
        self.sheet = compsheet
        self.partname = comppart
        self.location = "x99"
        self.name = "X"
        self.part = None

        self.sheet.add_component(self)

    def __str__(self):
        return "_".join((str(self.sheet), self.ref, self.partname, self.location, self.name))

    def __lt__(self, other):
        return self.name < other.name

    def add_node(self, node):
        ''' Add a node to this component '''
        self.nodes[node.pin.name] = node

class ComponentSexp(Component):

    ''' Create a `component` from netlist-sexp '''

    def __init__(self, board, sexp):
        i = sexp.find_first("sheetpath.names")
        sheet = board.sheets[board.pagename_to_sheet(i[0].name)]
        i = sexp.find_first("libsource.part")
        partname = i[0].name

        super().__init__(
            board,
            compref = sexp[0][0].name,
            compvalue = sexp.find_first("value")[0].name,
            compsheet = sheet,
            comppart = partname,
        )

        for i in sexp.find("property"):
            if i[0][0].name == 'Location':
                self.location = i[1][0].name
            elif i[0][0].name == 'Name':
                self.name = i[1][0].name

####################################################################################
#
# class VirtualComponent(Component):
#     ''' Components not instantiated in SystemC '''
#
#     def include_files(self):
#         if False:
#             yield None
#
#     def instance(self, file):
#         return
#
#     def initialize(self, file):
#         return
#
#     def hookup(self, file):
#         return
#
#
# class ModelComponent(Component):
#     ''' ModelComponents write their own SystemC implementation source code '''
#
#     busable = True
#
#
#     def __init__(self, board, sexp):
#         super().__init__(board, sexp)
#         self.lcname = None
#         self.ucname = None
#         self.modname = None
#         self.scm = None
#         self.fallback = False
#
#     def is_bus_ok(self, bus):
#         ''' Accept or reject a bus, and suggest optimal order '''
#         self.bus_signals = set()
#         for bname, bdata in self.bus_spec.items():
#             for i in range(bdata[0], bdata[1] + 1):
#                 self.bus_signals.add(bname + str(i))
#         self.configure()
#         mynodes = list(bus.mynodes(self))
#         busses = set()
#         for node in mynodes:
#             if node.pin.name not in self.bus_signals:
#                 print("NNN", self, node.pin.name, self.bus_signals)
#                 return False
#             busses.add(node.pin.name[0])
#         if len(busses) > 1:
#             print("MMM", self, busses)
#             return False
#         mynodes.sort(key=lambda x: (x.pin.name[0], int(x.pin.name[1:],10)))
#         bspec = self.bus_spec[list(busses)[0]]
#         return BusAttach(self, bus, order=mynodes, numeric=bspec[3], output=bspec[4])
#
#     def configure(self):
#         ''' A chance to do stuff before we start '''
#
#     def do_includes(self):
#         ''' This is where the action starts '''
#
#         self.configure()
#         if self.fallback:
#             super().do_includes()
#             return
#         self.make_clsname()
#         self.lcname = self.clsname.lower()
#         self.ucname = self.clsname.upper()
#         self.modname = self.sheet.board.name + "_" + self.ucname
#
#         old = self.sheet.board.dynamic_components.get(self.clsname)
#         if not old:
#             self.write_the_implementation()
#             self.sheet.board.dynamic_components[self.clsname] = self
#         else:
#             self.scm = old.scm
#
#         self.sheet.scm.hh.include(self.scm.hh)
#
#     def write_the_implementation(self):
#         ''' Write the code for this component '''
#         self.scm = self.sheet.board.sc_mod(self.modname)
#         self.write_code_hh(self.scm.hh)
#         self.write_code_cc(self.scm.cc)
#         self.scm.commit()
#         self.sheet.board.extra_scms.append(self.scm)
#
#     def write_code_cc(self, _file):
#         ''' Write the .cc file '''
#         print("WRONG TURN", self)
#         assert False
#
#     def buswidth(self, prefix):
#         ''' Return the width of one of our busses '''
#         blow, bhigh, _bdir, _bnum, _bout = self.bus_spec[prefix]
#         assert blow == 0
#         return 1 + bhigh - blow
#
#     def iter_nodes(self, name):
#         ''' iterate the nodes in a bus'''
#         width = self.buswidth(name)
#         for i in range(width):
#             sig = name
#             if width > 1:
#                 sig +=  "%d" % i
#             node = self.nodes[sig]
#             if (not node.net.bus) or node.net.bus.nets[0] == node.net:
#                 if node.net.bus:
#                     yield "BUS_%s" % sig, node
#                 else:
#                     yield "PIN_%s" % sig, node
#
#     def iter_signals(self, name):
#         ''' iterate the signals in a bus '''
#         for sig, _node in self.iter_nodes(name):
#             yield sig
#
#     def make_clsname(self):
#         ''' Nail down the class name and see of this model is sharable '''
#         signature = []
#         nodes = list(self.nodes.values())
#         for node in sorted(nodes):
#             bus = node.net.bus
#             if not bus and signature:
#                 prev = signature[-1]
#                 if prev[0] == 'P':
#                     signature[-1] = ('P', 1 + prev[1])
#                 else:
#                     signature.append(('P', 1))
#             elif not bus:
#                 signature.append(('P', 1))
#             elif bus.nets[0] == node.net and bus.numeric:
#                 signature.append(('N', 1, bus))
#             elif bus.nets[0] == node.net:
#                 signature.append(('B', 1, bus))
#             elif signature:
#                 prev = signature[-1]
#                 if prev[0] in 'BN' and prev[2] == bus:
#                     signature[-1] = (prev[0], 1 + prev[1], bus)
#                 else:
#                     signature.append(('b', 1, bus))
#         text = ''
#         for i in signature:
#             if i[1] > 1:
#                 text += i[0] + str(i[1])
#             else:
#                 text += i[0]
#         if 'b' in text:
#             # unordered bus(ses), invent a sharable signature if relevant
#             print("Signature", self, text, list(x.pin.name for x in sorted(self.nodes.values())))
#             self.clsname = self.name
#         else:
#             self.clsname = self.partname.upper() + "_" + text
#
#     def substitute(self, text):
#         ''' Substitute things into C-source text '''
#         if text[0] == '\n':
#             text = text[1:]
#         for find, replace in (
#             ("\t\t|", ""),
#             ("ccc", self.partname.lower()),
#             ("CCC", self.name),
#             ("VVV", self.value),
#             ("lll", self.lcname),
#             ("UUU", self.ucname),
#             ("MMM", self.modname),
#         ):
#             text = text.replace(find, replace)
#         return text
#
#     def write_bus_signals(self, file, typ, prefix):
#         ''' Write the signals for a bus '''
#         for sig, node in self.iter_nodes(prefix):
#             bus = node.net.bus
#             if not bus:
#                 file.write('\t%s <sc_logic>\t%s;\n' % (typ, sig))
#             elif bus.numeric:
#                 file.write('\t%s <uint64_t>\t%s;\n' % (typ, sig))
#             else:
#                 file.write('\t%s_rv<%d>\t\t%s;\n' % (typ, len(bus), sig))
#
#     def write_code_hh(self, file):
#         ''' Write the .hh file '''
#
#         file.write(self.substitute('''
# 		|
# 		|#ifndef R1000_MMM
# 		|#define R1000_MMM
# 		|
# 		|struct scm_lll_state;
# 		|
# 		|SC_MODULE(SCM_MMM)
# 		|{
# 		|'''))
#
#         self.write_code_hh_signals(file)
#         for bname, bdata in self.bus_spec.items():
#             assert bdata[0] == 0
#             self.write_bus_signals(file, bdata[2], bname)
#
#         file.write(self.substitute('''
# 		|
# 		|	SC_HAS_PROCESS(SCM_MMM);
# 		|
# 		|	SCM_MMM(sc_module_name nm, const char *arg);
# 		|
# 		|	private:
# 		|
# 		|	struct scm_lll_state *state;
# 		|	void doit(void);
# 		|'''))
#
#         self.write_code_hh_extra_private(file)
#
#         file.write(self.substitute('''
# 		|};
# 		|
# 		|#endif /* R1000_MMM */
# 		|'''))
#
#     def write_code_hh_signals(self, _file):
#         ''' write the list of other SC signals '''
#         return
#
#     def write_code_cc_init(self, file):
#         ''' Write the top of the .cc file, including the initializer '''
#
#         file.write(self.substitute('''
# 		|
# 		|#include <systemc.h>
# 		|#include "Chassis/r1000sc.h"
# 		|#include "Infra/context.h"
# 		|
# 		|'''))
#
#         self.scm.cc.include(self.scm.hh)
#
#         file.write(self.substitute('''
# 		|
# 		|struct scm_lll_state {
# 		|	struct ctx ctx;
# 		|	uint64_t data;
# 		|	int job;
# 		|'''))
#
#         self.write_code_cc_state_extra(file)
#
#         file.write(self.substitute('''
# 		|};
# 		|
# 		|SCM_MMM :: SCM_MMM(sc_module_name nm, const char *arg) : sc_module(nm)
# 		|{
# 		|	state = (struct scm_lll_state *)
# 		|	    CTX_Get("ccc", this->name(), sizeof *state);
# 		|	should_i_trace(this->name(), &state->ctx.do_trace);
# 		|'''))
#
#         self.write_code_cc_init_extra(file)
#
#         file.write(self.substitute('''
# 		|
# 		|	SC_METHOD(doit);
# 		|	sensitive'''))
#
#         self.write_code_cc_sensitive(file)
#
#         file.write(";\n")
#         file.write('}\n\n')
#
#     def write_code_cc_sensitive(self, _file):
#         ''' Write the sensitivity list '''
#         assert False
#
#     def write_code_cc_init_extra(self, file):
#         ''' Used to add extra initialization '''
#
#     def write_code_cc_state_extra(self, file):
#         ''' Used to add variables to state structure '''
#
#     def write_code_hh_extra_private(self, file):
#         ''' Used to add mode private fields '''
#
#     def write_sensitive_bus(self, file, prefix):
#         ''' Write sensitivey lines for a bus '''
#         for sig, node in self.iter_nodes(prefix):
#             if node.net.bus:
#                 file.write("\n\t    << %s" % sig)
#             else:
#                 file.write("\n\t    << %s" % sig)
#
#     def bus_is_sequential(self, bus):
#         ''' Are the bits in the bus sequential for our bus ? '''
#         offset = set()
#         for idx, bnode in enumerate(bus.mynodes(self)):
#             offset.add(int(bnode.pin.name[1:], 10) - idx)
#         offset = list(offset)
#         return len(offset) == 1, offset[0]
#
#     def read_bus_value(self, var, prefix):
#         ''' Read value of bus '''
#         width = self.buswidth(prefix)
#         yield '%s = 0;' % var
#         for sig, node in self.iter_nodes(prefix):
#             assert node.pin.name[:len(prefix)] == prefix
#             if width == 1:
#                 bit = 0
#             else:
#                 bit = width - int(node.pin.name[len(prefix):]) - 1
#             bus = node.net.bus
#             if not bus:
#                 yield 'if (IS_H(%s)) %s |= (1ULL << %d);' % (sig, var, bit)
#                 continue
#             bus_seq, bus_off = self.bus_is_sequential(bus)
#
#             if not bus.numeric:
#                 for idx, bnode in enumerate(bus.mynodes(self)):
#                     assert bnode.pin.name[:len(prefix)] == prefix
#                     bit = width - int(bnode.pin.name[len(prefix):]) - 1
#                     bidx = len(bus) - idx - 1
#                     yield 'if (IS_H(%s.read()[%d])) %s |= (1ULL << %d);' % (sig, bidx, var, bit)
#                 continue
#
#             if bus_seq:
#                 busmask = (1 << len(bus)) - 1
#                 bshift = width - len(bus) - bus_off
#                 yield '%s |= (%s.read() & 0x%xULL) << %d;' % (var, sig, busmask, bshift)
#                 continue
#
#             for idx, bnode in enumerate(bus.mynodes(self)):
#                 assert bnode.pin.name[:len(prefix)] == prefix
#                 bit = width - int(bnode.pin.name[len(prefix):]) - 1
#                 bidx = len(bus) - idx - 1
#                 yield 'if (%s.read() & (1ULL << %d)) %s |= (1ULL << %d);' % (sig, bidx, var, bit)
#
#     def write_bus_z(self, prefix):
#         ''' Set bus to sc_logic_Z '''
#         for sig, node in self.iter_nodes(prefix):
#             if not node.net.bus:
#                 yield '%s = sc_logic_Z;' % sig
#             else:
#                 yield '%s.write("%s");' % (sig, 'Z' * len(node.net.bus))
#
#     def write_bus_val(self, prefix, var):
#         ''' Write a value to a bus '''
#         width = self.buswidth(prefix)
#         for sig, node in self.iter_nodes(prefix):
#             assert node.pin.name[:len(prefix)] == prefix
#             if width == 1:
#                 bit = 0
#             else:
#                 bit = width - int(node.pin.name[len(prefix):]) - 1
#             bus = node.net.bus
#
#             if not bus:
#                 yield '%s = AS(%s & (1ULL << %d));' % (sig, var, bit)
#                 continue
#             bus_seq, bus_off = self.bus_is_sequential(bus)
#             tmpname = sig + "_tmp"
#             if len(bus) == width:
#                 good = True
#                 for idx, bnode in enumerate(bus.mynodes(self)):
#                     if bnode.pin.name != prefix + "%d" % idx:
#                         good = False
#                         break
#                 if good:
#                     yield '%s.write(%s);' % (sig, var)
#                     continue
#             if not bus.numeric:
#                 yield '{'
#                 yield '\tsc_lv<%d> %s;' % (len(bus), tmpname)
#                 for idx, bnode in enumerate(bus.mynodes(self)):
#                     assert bnode.pin.name[:len(prefix)] == prefix
#                     bit = width - int(bnode.pin.name[len(prefix):]) - 1
#                     bidx = len(bus) - idx - 1
#                     yield '\t%s[%d] = AS(%s & (1ULL << %d));' % (tmpname, bidx, var, bit)
#                 yield '%s.write(%s);' % (sig, tmpname)
#                 yield '}'
#                 continue
#
#             if bus_seq:
#                 busmask = (1 << len(bus)) - 1
#                 bshift = width - len(bus) - bus_off
#                 yield '%s.write((%s >> %d) & 0x%xULL);' % (sig, var, bshift, busmask)
#                 continue
#
#             yield '{'
#             yield '\tuint64_t %s = 0;' % tmpname
#             for idx, bnode in enumerate(bus.mynodes(self)):
#                 assert bnode.pin.name[:len(prefix)] == prefix
#                 bit = width - int(bnode.pin.name[len(prefix):]) - 1
#                 bidx = len(bus) - idx - 1
#                 yield '\tif (%s & (1ULL << %d)) %s |= (1ULL << %d);' % (var, bit, tmpname, bidx)
#             yield '\t%s.write(%s);' % (sig, tmpname)
#             yield '}'
#
#     def instance(self, file):
#         ''' Write the instance for the .hh file '''
#         if self.fallback:
#             super().instance(file)
#             return
#         file.write(self.substitute('\tSCM_MMM CCC;\n'))
#
#     def initialize(self, file):
#         ''' Initialize the instance in the .cc file '''
#         if self.fallback:
#             super().initialize(file)
#             return
#         file.write(self.substitute(',\n\tCCC("CCC", "VVV")'))
#
#     def hookup_model(self, file):
#         ''' Where the signals are hooked up to SystemC '''
#
#     def hookup(self, file):
#         if self.fallback:
#             super().hookup(file)
#             return
#         file.write("\n\n\t// %s\n" % " ".join((self.ref, self.name, self.location, self.partname)))
#         self.hookup_model(file)
#
#     def hookup_bus(self, file, prefix):
#         ''' Hook this bus into the SC simulation '''
#         for sig, node in self.iter_nodes(prefix):
#             bus = node.net.bus
#             if not bus:
#                 file.write('\t%s.%s(%s);\n' % (self.name, sig, node.net.cname))
#             else:
#                 file.write('\t%s.%s(%s);\n' % (self.name, sig, bus.busname))
