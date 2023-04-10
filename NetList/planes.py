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
import util

class PlaneSignal():
    ''' A signal on a plane collected from G[BF] parts '''

    def __init__(self, cpu, name):
        self.cpu = cpu
        self.planename = name
        self.name = name
        self.sortkey = util.sortkey(name)
        self.nets = []
        self.net = None
        self.boards = {}
        self.is_supply = False
        self.defval = None

    def __repr__(self):
        rval = []
        rval.append(self.name)
        rval.append(self.net.bcname)
        for board in self.cpu.boards:
            net = self.boards.get(board.name)
            if net is not None:
                nname = net.name.split('/')[-1]
                rval.append(nname)
            else:
                rval.append("-")
        return "".join(x.ljust(19) for x in rval)

    def __lt__(self, other):
        return self.sortkey < other.sortkey

    def add_net(self, net):
        ''' Add a network to this plane signal '''
        self.is_supply |= net.is_supply
        self.nets.append(net)
        if net.board in self.boards:
            print(
                "Board has multiple nets on plane",
                self.name,
                "from",
                net.board,
                self.boards[net.board],
                net
            )
        self.boards[net.board.name] = net

    def chew(self):
        ''' Chew on things '''
        if self.is_supply and self.name not in {"PD", "PU"}:
            return
        self.divine_better_name()
        self.sortkey = util.sortkey(self.name)
        self.net = self.nets.pop(0)
        self.net.is_plane = self
        self.net.board.del_net(self.net)
        self.net.name = self.name
        self.net.board = None
        self.cpu.nets[self.name] = self.net
        for net in self.nets:
            self.net.adopt(net)

        if self.defval is not None:
            self.net.default = self.defval

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
        print("Plane signal name divination failed", self.name, self.boards)

class Planes():

    ''' {Front|Back}-planes '''

    def __init__(self, cpu):
        self.cpu = cpu
        self.psig = {}
        self.scm = self.cpu.sc_mod("planes")

        for i in ("PU", "PD"):
            self.psig[i] = PlaneSignal(cpu, i)

    def build_planes(self):
        ''' Find the plane signals '''
        for board in self.cpu.boards:
            for comp in (board.iter_components()):
                if not comp.is_plane:
                    continue
                if not comp.nodes:
                    continue

                node = comp.nodes['W']		# The pin name of GB/GF parts
                net = node.net
                node.remove()

                oref = comp.ref[:2] + "%03d" % int(comp.ref[2:], 10)
                nref, defval = transit.do_transit(board.name, oref)

                psig = self.psig.get(nref)
                if not psig:
                    psig = PlaneSignal(self.cpu, nref)
                    self.psig[nref] = psig
                psig.add_net(net)
                psig.defval = defval

        for psig in self.psig.values():
            psig.chew()

    def produce(self):
        ''' Produce the SystemC sources '''
        for psig in sorted(self.psig.values()):
            for sig in psig.net.sc_signals():
                self.scm.add_signal(sig)

        self.make_table(self.scm.sf_cc)
        self.scm.add_member("sc_trace_file *tf;")
        self.scm.emit_pub_hh()
        self.scm.emit_hh()
        self.scm.emit_cc()
        self.scm.commit()

    def make_table(self, dst):
        ''' Document the resulting planes in C comment '''
        dst.write("//".ljust(47))
        dst.write("".join(x.name.ljust(19) for x in self.cpu.boards) + "\n")
        for signame, psig in sorted(self.psig.items()):
            if psig.is_supply:
                dst.write("// " + str(signame) + " <supply>\n")
            else:
                dst.write("// " + str(signame) + " " + str(psig) + "\n")
