#!/usr/local/bin/python3
#
# Copyright (c) 2022 Poul-Henning Kamp
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
   Pass: Deal with components which are plane-connections
   ======================================================
'''

import transit
import srcfile

class PlaneSignal():
    ''' ... '''

    def __init__(self, cpu, name):
        self.cpu = cpu
        self.name = name
        self.nets = []
        self.boards = {}
        self.roles = {}
        self.is_supply = False

    def __repr__(self):
        l = []
        for board in self.cpu.boards:
            net = self.boards.get(board)
            if net:
                nname = net.name.split('/')[-1]
                l.append(nname)
            else:
                l.append("-")
        return "".join("%-19s" % x for x in l) + " - " + str(self.roles)

    def add_net(self, net):
        self.nets.append(net)
        self.boards[net.board] = net

    def chew(self):
        for board in self.cpu.boards:
            net = self.boards.get(board)
            if net:
                for node in net.nnodes:
                    if node.component.is_plane:
                        continue
                    if node.pin.role not in self.roles:
                        self.roles[node.pin.role] = 0
                    self.roles[node.pin.role] += 1
        self.is_supply = not len(self.roles)

class PassPlanes():

    ''' Pass: Deal with connections to the planes '''

    def __init__(self, cpu):
        self.cpu = cpu
        self.psig = {}
        self.pfx = "Chassis/%s/planes" % cpu.branch
        self.cfile = srcfile.SrcFile(self.pfx + ".cc")
        self.hfile = srcfile.SrcFile(self.pfx + ".hh")

        for board in self.cpu.boards:
            for comp in board.iter_components():
                comp.is_plane = comp.partname in ("GF", "GB")
                if not comp.is_plane:
                    continue
                comp.ref = comp.ref[:2] + "%03d" % int(comp.ref[2:], 10)
                nref = transit.do_transit(board.name, comp.ref)
                comp.ref = nref
                comp.name = nref
                if nref not in self.psig:
                    self.psig[nref] = PlaneSignal(cpu, nref)
                self.psig[nref].add_net(comp.nodes['W'].net)

        for psig in self.psig.values():
            psig.chew()

        self.make_hfile()
        self.make_cfile()
        self.make_makefile()

    def make_hfile(self):
        ''' ... '''

        for alias, gb in sorted(transit.GB.items()):
            self.hfile.write("#define GB_%s %s\n" % (alias, gb))

        self.hfile.fmt('''
		|
		|SC_MODULE(mod_planes)
		|{
		|''')

        for signame, psig in sorted(self.psig.items()):
            if psig.is_supply:
                continue
            self.hfile.write('\tsc_signal_resolved %s;\n' % signame)

        self.hfile.fmt('''
		|	sc_signal_resolved PU;
		|	sc_signal_resolved PD;
		|
		|	sc_trace_file *tf;
		|
		|	mod_planes(sc_module_name name);
		|};
		|
		|struct mod_planes *make_mod_planes(sc_module_name name);
		|''')
        self.hfile.commit()

    def make_cfile(self):
        ''' ... '''

        for signame, psig in sorted(self.psig.items()):
            if psig.is_supply:
                self.cfile.write("// " + str(signame) + " <supply>\n")
            else:
                self.cfile.write("// " + str(signame) + " " + str(psig) + "\n")

        self.cfile.fmt('''
		|
		|#include <systemc.h>
		|
		|''')

        self.cfile.include(self.hfile)

        self.cfile.fmt('''
		|
		|struct mod_planes *make_mod_planes(sc_module_name name)
		|{
		|	return new mod_planes(name);
		|}
		|
		|mod_planes :: mod_planes(sc_module_name name) :
		|	sc_module(name)
		|''')

        for signame, psig in sorted(self.psig.items()):
            if psig.is_supply:
                continue
            self.cfile.write('\t,%s("%s", sc_logic_1)\n' % (signame, signame))

        self.cfile.fmt('''
		|	,PU("PU", sc_logic_1)
		|	,PD("PD", sc_logic_0)
		|{
		|}
		|''')

        self.cfile.commit()

    def make_makefile(self):
        mf = self.cpu.chassis_makefile
        mf.write("OBJS += %s.o\n" % self.pfx)
        mf.write("%s.o: \\\n" % self.pfx)
        mf.write("    %s.cc \\\n" % self.pfx)
        mf.write("    %s.hh\n" % self.pfx)
        mf.write("\t${SC_CC} -o %s.o %s.cc\n" % (self.pfx, self.pfx))
		
