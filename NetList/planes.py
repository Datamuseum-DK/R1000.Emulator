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
   {Front|Back}-Plane
   ==================
'''

import transit
import srcfile

class PlaneSignal():
    ''' ... '''

    def __init__(self, cpu, name):
        self.cpu = cpu
        self.planename = name
        self.name = name
        self.nets = []
        self.net = None
        self.boards = {}
        self.is_supply = False

    def __repr__(self):
        l = []
        l.append(self.name)
        l.append(self.net.bcname)
        for board in self.cpu.boards:
            net = self.boards.get(board.name)
            if net is not None:
                nname = net.name.split('/')[-1]
                l.append(nname)
            else:
                l.append("-")
        return "".join(x.ljust(19) for x in l)

    def add_net(self, net):
        self.is_supply |= net.is_supply
        self.nets.append(net)
        if net.board in self.boards:
            print("Multiple nets on plane", self.name, "from", net.board, self.boards[net.board], net)
        self.boards[net.board.name] = net

    def chew(self):
        if self.is_supply and self.name not in {"PD", "PU"}:
            return
        self.divine_better_name()
        self.net = self.nets.pop(0)
        self.net.is_plane = self
        self.net.board.del_net(self.net)
        self.net.name = self.name
        self.net.board = None
        self.cpu.nets[self.name] = self.net
        for net in self.nets:
            self.net.adopt(net)

    def divine_better_name(self):
        ''' Divine a better name '''

        # Entirely heuristic…
        # EMU we control
        # SEQ seems to have more communicative names
        for pfx, bname in (
            (None, "EMU"),
            ("Q.", "SEQ"),
            ("I.", "IOC"),
            ("F.", "FIU"),
            ("T.", "TYP"),
            (None, "MEM32"),
            (None, "SEQ"),
        ):
            net = self.boards.get(bname)
            if not net:
                continue
            nname = net.name.split('/')[-1]
            if not pfx:
                self.name = nname
                return
            if nname[:2] == pfx:
                self.name = nname[2:]
                return

        # If there is only one net, use that.
        if len(self.nets) == 1:
            self.name = self.nets[0].name.split('/')[-1]
            return

        # If all else fails, continue with G[BF]%03d
        print("Divination failed", self.name, self.boards)

class Planes():

    ''' {Front|Back}-planes '''

    def __init__(self, cpu):
        self.cpu = cpu
        self.psig = {}
        self.pfx = "Chassis/%s/planes" % cpu.branch
        self.cfile = srcfile.SrcFile(self.pfx + ".cc")
        self.hfile = srcfile.SrcFile(self.pfx + ".hh")

        for i in ("PU", "PD"):
            self.psig[i] = PlaneSignal(cpu, i)

    def build_planes(self):
        for board in self.cpu.boards:
            for comp in list(board.iter_components()):
                if not comp.is_plane:
                    continue
                if not comp.nodes:
                    continue

                node = comp.nodes['W']
                net = node.net
                node.remove()

                comp.ref = comp.ref[:2] + "%03d" % int(comp.ref[2:], 10)
                nref = transit.do_transit(board.name, comp.ref)
                comp.ref = nref
                comp.name = nref

                if nref not in self.psig:
                    self.psig[nref] = PlaneSignal(self.cpu, nref)
                self.psig[nref].add_net(net)

        for psig in self.psig.values():
            psig.chew()

    def produce(self):

        self.make_hfile()
        self.make_cfile()
        self.make_makefile()

    def make_hfile(self):
        ''' ... '''

        self.hfile.fmt('''
		|
		|SC_MODULE(mod_planes)
		|{
		|''')

        for signame, psig in sorted(self.psig.items()):
            psig.net.write_decl(self.hfile)

        self.hfile.fmt('''
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

        self.cfile.write("//".ljust(47) + "".join(x.name.ljust(19) for x in self.cpu.boards) + "\n")
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
            psig.net.write_init(self.cfile)

        self.cfile.fmt('''
		|{
		|}
		|''')

        self.cfile.commit()

    def make_makefile(self):
        bname = self.pfx.split("/")[-1]
        mf = self.cpu.chassis_makefile
        mf.write("OBJS += ${OBJDIR}/%s.o\n" % bname)
        mf.write("${OBJDIR}/%s.o: \\\n" % bname)
        mf.write("    %s.cc \\\n" % self.pfx)
        mf.write("    %s.hh\n" % self.pfx)
        mf.write("\t${SC_CC} -o ${OBJDIR}/%s.o %s.cc\n" % (bname, self.pfx))
		
