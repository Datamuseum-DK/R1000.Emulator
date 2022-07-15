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

MIN_BUS_WIDTH = 8

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

    def first_node(self, comp):
        return self.nodes[comp][self.nets[0]]

    def sort_nets(self):
        pivot_node = self.nets[0].nnodes[0]
        nodes = self.components[pivot_node.component]
        nodes.sort(key=lambda x: x.pin.sortkey)
        self.nets = list(x.net for x in nodes)
        for idx, net in enumerate(self.nets):
            for node in net.nnodes:
                node.pin.netbusidx = idx

    def invalid(self, file):
        sc_type = set(x.sc_type for x in self.nets)
        if len(sc_type) > 1:
            file.write("\nMixed bus " + str(sc_type) + "\n")
            self.table(file, "\t")
            return True
        for comp in self.components:
            if not comp.part.busable:
                file.write("\nComponent not busable " + comp.part.name + " " + comp.board.name + " " + comp.name + "\n")
                self.table(file, "\t")
                return True
        return False

    def unordered(self, file):
        for comp in self.components:
            sks = list(self.nodes[comp][net].pin.sortkey for net in self.nets)
            spread = 1 + sks[-1][-1] - sks[0][-1]
            if sks != sorted(sks) or spread != len(self.nets):
                if not self.searching:
                    file.write("\nBus pins out of order " + comp.part.name + " " + comp.board.name + " " + comp.name + "\n")
                    self.table(file, "\t")
                if len(self.nets) > 2 and len(self.nets) - 1 > len(self.best):
                    self.searching += 1
                    orig = self.nets
                    self.nets = orig[1:]
                    self.unordered(file)
                    self.nets = orig[:-1]
                    self.unordered(file)
                    self.nets = orig
                    self.searching -= 1
                if not self.searching and self.best:
                    orig = self.nets
                    self.nets = self.best
                    file.write("\nSubset found \n")
                    self.table(file, "\t")
                    self.nets = orig
                return True
        if len(self.nets) > len(self.best):
            self.best = self.nets
        return False

    def table(self, file, pfx=""):
        self.decide_cname()
        file.write(pfx + "BUS")
        file.write(" %d×%d" % (len(self.nets), len(self.nodes)))
        file.write(" \t" + str(self.cname) + "\n")
        file.write(pfx + "   [" + self.sig + "]\n")

        i = [""]
        for component in self.components:
            i.append(component.board.name)
        file.write(pfx + "\t".join(i) + "\n")

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
            if net.sc_type == "bool":
                file.write("\tsc_signal <uint64_t> %s;\n" % lname)
            else:
                file.write("\tsc_signal_rv <%d> %s;\n" % (len(self.nets), lname))

    def write_init(self, net, file):
        if net == self.nets[0]:
            lname = self.cname.split(".")[-1]
            if net.sc_type == "bool":
                file.write(',\n\t%s("%s", (1ULL << %d) - 1)' % (lname, lname, len(self.nets)))
            else:
                file.write(',\n\t%s("%s", "%s")' % (lname, lname, "z" * len(self.nets)))

class PassNetConfig():

    ''' Pass: Configure the `net` '''

    def __init__(self, cpu):
        self.cpu = cpu
        self.netbusses = {}

        for _gnam, net in sorted(self.cpu.nets.items()):
            for node in net.iter_nodes():
                net.sheets.add(node.component.sheet)
            net.sheets = list(sorted(net.sheets))
            net.is_local = not net.is_plane and len(net.sheets) == 1
            if net.is_local:
                net.sheets[0].local_nets.append(net)
            net.find_cname()

        self.ponder_bool()
        with open("_bus_%s.txt" % self.cpu.branch, "w") as file:
            self.bus_candidates(file)

    def ponder_bool(self):
        for _gnam, net in sorted(self.cpu.nets.items()):
            if net.no_bool:
                continue
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
        for _gnam, net in sorted(self.cpu.nets.items()):

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
            sig += " ".join(sorted(x.component.board.name + ":" + x.component.ref + ":" + x.pin.pinbus.name for x in net.nnodes))
            i = self.netbusses.get(sig)
            if i:
                i.add_net(net)
            else:
                self.netbusses[sig] = NetBus(sig, net)

        for sig, maybebus in list(self.netbusses.items()):
            maybebus.sort_nets()
            if len(maybebus.nets) < 2:
                del self.netbusses[sig]
                continue
            if maybebus.invalid(file):
                del self.netbusses[sig]
                continue

        accepted = []
        for sig, maybebus in self.netbusses.items():
            while len(maybebus.nets) >= MIN_BUS_WIDTH:
                maybebus.sort_nets()
                maybebus.best = []
                maybebus.searching = 0
                if not maybebus.unordered(file):
                    accepted.append(maybebus)
                    break
                if len(maybebus.best) < 2:
                    break

                target = set(maybebus.best)
                rest = set(maybebus.nets) - target
                target = list(target)
                rest = list(rest)

                newbus = NetBus(maybebus.sig, target.pop(0))
                while target:
                    newbus.add_net(target.pop(0))

                maybebus = NetBus(maybebus.sig, rest.pop(0))
                while rest:
                    maybebus.add_net(rest.pop(0))

        file.write("\n")
        file.write("Accepted busses\n")
        file.write("===============\n")
        for netbus in accepted:
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
