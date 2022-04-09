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
   Pass: Configure networks
   ========================
'''

class MaybeBus():

    ''' ... '''

    def __init__(self, sig, net):
        self.sig = sig
        self.nodes = []
        self.components = {}
        self.nets = []
        self.nodes = {}
        self.failed = len(net.nnodes) < 2

        if self.failed:
            return
        self.nets.append(net)
        for node in net.nnodes:
            if not node.pin.bus:
                self.failed = True
                return
            self.components[node.component] = node.pin.bus
            self.nodes[node.component] = [node]

    def add_net(self, net):
        if self.failed:
            return
        self.nets.append(net)
        for node in net.nnodes:
            i = self.components.get(node.component)
            if not i:
                self.failed = True
                return
            if i != node.pin.bus:
                self.failed = True
                return
            self.nodes[node.component].append(node)

    def table(self, file):
        file.write("BUS %d×%d\n" % (len(self.nets), len(self.nodes)))

        i = [""]
        for component in self.components:
            i.append(component.name)
        file.write("\t".join(i) + "\n")

        for net in self.nets:
            i = [""]
            for node in net.nnodes:
                i.append(node.pin.name)
            i.append(net.name)
            file.write("\t".join(i) + "\n")


class PassNetConfig():

    ''' Pass: Configure the `net` '''

    def __init__(self, board):
        self.board = board

        for net in self.board.iter_nets():
            net.is_plane = net.name in ("PU", "PD")
            for node in net.iter_nodes():
                if node.component.partname in ("GF", "GB"):
                    net.is_plane = True
                    net.name = node.component.ref
                else:
                    net.sheets.add(node.component.sheet)
            net.sheets = list(sorted(net.sheets))
            net.is_local = not net.is_plane and len(net.sheets) == 1
            if net.is_local:
                net.sheets[0].local_nets.append(net)
            net.find_cname()

        self.ponder_bool()
        self.ponder_bus()

    def ponder_bool(self):
        for net in self.board.iter_nets():
            i = {}
            for node in net.iter_nodes():
                i[node.pin.role] = 1 + i.setdefault(node.pin.role, 0)
            if len(i) == 1 and ('c_input' in i or 'c_output' in i):
                # print(net, '=>', "bool", "(unconnected)")
                net.sc_type = "bool"
                continue
            if 'c_input' not in i or 'c_output' not in i or len(i) > 2:
                self.assign_blame(net)
                continue
            if i['c_output'] > 1:
                continue
            # print(net, '=>', "bool")
            net.sc_type = "bool"

    def ponder_bus(self):
        with open("_bus_%s.txt" % self.board.name.lower(), "w") as file:
            maybe = {}
            for net in self.board.iter_nets():
                if len(set(net.nnodes)) != len(net.nnodes):
                    continue
                sig = " ".join(x.component.ref for x in net.nnodes)
                i = maybe.get(sig)
                if i:
                    i.add_net(net)
                else:
                    maybe[sig] = MaybeBus(sig, net)
            for sig, maybebus in maybe.items():
                if len(maybebus.nets) < 2:
                    maybebus.failed = True
                if not maybebus.failed:
                    maybebus.table(file)

    def assign_blame(self, net):
        if len(net) < 2 or net.is_plane:
            return
        i = {}
        for node in net.iter_nodes():
            if node.pin.role in (
                "sc_inout_resolved",
                "tri_state",
                "bidirectional",
            ):
                return
            if node.pin.role in (
                "c_input",
                "c_output",
            ):
                continue
            i[node.component.part] = 1 + i.setdefault(node.component.part, 0)
        if len(i) == 1:
            part, _count = i.popitem()
            # print("BLAME", net, part)
            part.blame.add(net)
