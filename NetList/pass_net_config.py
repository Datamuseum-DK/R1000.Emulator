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

import util
import sys

class NetBus():

    ''' ... '''

    def __init__(self, sig, net):
        self.sig = sig
        self.nodes = []
        self.components = {}
        self.nets = []
        self.nodes = {}
        self.cname = None

        self.nets.append(net)
        for node in net.nnodes:
            self.components[node.component] = [node]
            self.nodes[node.component] = {node.net: node}

    def __repr__(self):
        return "<MBUS %d×%d>" % (len(self.nets), len(self.nodes))

    def __len__(self):
        return len(self.nets)

    def add_net(self, net):
        self.nets.append(net)
        for node in net.nnodes:
            self.components[node.component].append(node)
            self.nodes[node.component][node.net] = node

    def sort_nets(self):
        pivot_node = self.nets[0].nnodes[0]
        nodes = self.components[pivot_node.component]
        nodes.sort(key=lambda x: x.pin.sortkey)
        self.nets = list(x.net for x in nodes)

    def invalid(self, file):
        self.sort_nets()
        for comp in self.components:
            if not comp.part.busable:
                file.write("\nComponent not busable " + comp.part.name + "\n")
                self.table(file, "\t")
                return True
            sks = list(self.nodes[comp][net].pin.sortkey for net in self.nets)
            spread = 1 + sks[-1][-1] - sks[0][-1]
            if sks != sorted(sks):
                file.write("\nBus pins out of order " + comp.name + "\n")
                self.table(file, "\t")
                return True
            if spread != len(self.nets):
                file.write("\nBus pins out of order " + comp.name + "\n")
                self.table(file, "\t")
                return True
        sc_type = set(x.sc_type for x in self.nets)
        if len(sc_type) > 1 or "bool" not in sc_type:
            file.write("\nBus is not boolen " + str(sc_type) + "\n")
            self.table(file, "\t")
            return True
        return False

    def table(self, file, pfx=""):
        self.decide_cname()
        file.write(pfx + "BUS %d×%d\t%s\n" % (len(self.nets), len(self.nodes), str(self.cname)))
        file.write(pfx + "   [" + self.sig + "]\n")

        i = [""]
        for component in self.components:
            i.append(component.name)
        file.write(pfx + "\t".join(i) + "\n")

        i = [""]
        for node in self.nets[0].nnodes:
            i.append(
                {
                "output": "out",
                "c_output": "out",
                "input": "in",
                "c_input": "in",
                "tri_state": "zo",
                "bidirectional": "zio",
                "sc_inout_resolved": "zio",
                "sc_out <sc_logic>": "zo",
                }[node.pin.role]
            )
        file.write(pfx + "\t".join(i) + "\n")

        for net in self.nets:
            i = [""]
            for node in net.nnodes:
                i.append(node.pin.name)
            i.append(net.name)
            file.write(pfx + "\t".join(i) + "\n")

    def decide_cname(self):
        b = str(util.sortkey(self.nets[-1].cname)[-1]).split(".")[-1]
        self.cname = self.nets[0].cname + "_to_" + b

    def register(self):
        self.decide_cname()
        self.sort_nets()
        for net in self.nets:
            net.netbus = self
            for node in net.nnodes:
                node.netbus = self

    def write_decl(self, net, file):
        if net == self.nets[0]:
            lname = self.cname.split(".")[-1]
            file.write("\tsc_signal <uint64_t> %s;\n" % lname)

    def write_init(self, net, file):
        if net == self.nets[0]:
            lname = self.cname.split(".")[-1]
            file.write(",\n\t%s(\"%s\", (1ULL << %d) - 1)" % (lname, lname, len(self.nets)))

class PassNetConfig():

    ''' Pass: Configure the `net` '''

    def __init__(self, board):
        self.board = board
        self.netbusses = {}

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
        with open("_bus_%s.txt" % self.board.name.lower(), "w") as file:
            self.bus_candidates(file)

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

    def bus_candidates(self, file):
        for net in self.board.iter_nets():

            if len(net.nnodes) < 2:
                continue

            uniq_comps = set(x.component.ref for x in net.nnodes)
            if len(uniq_comps) != len(net.nnodes):
                # Signals which connect multiple times to a single component
                # are disqualified, because we cannot (easily) figure out
                # which of the multiple connections to assign where in the bus.
                continue

            if None in (x.pin.pinbus for x in net.nnodes):
                continue

            sig = " ".join(str(x) for x in net.sortkey[:-1]) + " "
            sig += " ".join(sorted(x.component.ref + ":" + x.pin.pinbus.name for x in net.nnodes))
            i = self.netbusses.get(sig)
            if i:
                i.add_net(net)
            else:
                self.netbusses[sig] = NetBus(sig, net)

        for sig, maybebus in list(self.netbusses.items()):
            if len(maybebus.nets) < 2 or maybebus.invalid(file):
                del self.netbusses[sig]

        file.write("\n")
        file.write("Accepted busses\n")
        file.write("===============\n")
        for netbus in self.netbusses.values():
            netbus.register()
            file.write("\n")
            netbus.table(file)

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
