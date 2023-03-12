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

MIN_BUS_WIDTH = 4

class NetBus():

    ''' ... '''

    def __init__(self, sig, net):
        self.sig = sig
        self.nodes = []
        self.components = {}
        self.nets = []
        self.nodes = {}
        self.cname = None
        self.ctype = None
        self.best = None

        self.nets.append(net)
        for node in net.nnodes:
            key = (node.component, node.pin.pinbus)
            self.components[key] = [node]
            self.nodes[key] = {node.net: node}

    def __repr__(self):
        return "<MBUS %d×%d>" % (len(self.nets), len(self.nodes))

    def __len__(self):
        return len(self.nets)

    def add_net(self, net):
        ''' Add another net '''
        self.nets.append(net)
        for node in net.nnodes:
            key = (node.component, node.pin.pinbus)
            self.components[key].append(node)
            self.nodes[key][node.net] = node

    def sort_nets(self):
        ''' Sort the nets into (pressumed) bus-order '''
        pivot_node = self.nets[0].nnodes[0]
        pivot_key = (pivot_node.component, pivot_node.pin.pinbus)
        nodes = self.components[pivot_key]
        nodes.sort(key=lambda x: x.pin.sortkey)
        self.nets = list(x.net for x in nodes)
        for idx, net in enumerate(self.nets):
            for node in net.nnodes:
                node.pin.netbusidx = idx

    def invalid(self, file):
        ''' Detect mixed signal types and non-busable components '''
        sc_type = set(x.sc_type for x in self.nets)
        if len(sc_type) > 1:
            file.write("\nMixed bus " + str(sc_type) + "\n")
            self.table(file, "\t")
            return True
        for comp, _pinbus in self.components:
            if not comp.part.busable:
                file.write("\nComponent not busable ")
                file.write(comp.part.name + " " + comp.board.name + " " + comp.name + "\n")
                self.table(file, "\t")
                return True
        return False

    def is_valid_bus(self, nets):
        ''' Test if nets constitute a proper bus '''
        for key in self.components:
            sks = list(self.nodes[key][net].pin.sortkey for net in nets)
            spread = 1 + sks[-1][-1] - sks[0][-1]
            if sks != sorted(sks) or spread != len(nets):
                return False
        return True

    def unordered(self):
        ''' Find the largest bus, if any, in self.nets '''
        best = (0, 0, [])
        i = 0
        anets = list(self.nets)
        while len(anets) >= MIN_BUS_WIDTH:
            tnets = list(anets)
            j = 1
            while len(tnets) >= MIN_BUS_WIDTH:
                if not self.is_valid_bus(tnets):
                    tnets.pop(-1)
                    continue
                if len(tnets) > best[0]:
                    best = (len(tnets), i, tnets)
                j = len(tnets)
                break
            anets = anets[j:]
        if best[2]:
            self.best = best[2]
        return len(best[2]) != len(self.nets)

    def table(self, file, pfx=""):
        ''' render in table format '''
        self.decide_cname()
        file.write(pfx + "BUS")
        file.write(" %d×%d" % (len(self.nets), len(self.nodes)))
        file.write(" \t" + str(self.cname) + " " + str(self.ctype) + "\n")
        file.write(pfx + "   [" + self.sig + "]\n")

        i = [""]
        for component, _pinbus in self.components:
            i.append(component.board.name)
        file.write(pfx + "\t".join(i) + "\n")

        i = [""]
        for component, _pinbus in self.components:
            i.append(component.name)
        file.write(pfx + "\t".join(i) + "\n")

        i = [""]
        for node in self.nets[0].nnodes:
            i.append(node.pin.type.name)
        file.write(pfx + "\t".join(i) + "\n")

        for net in sorted(self.nets):
            i = [""]
            for node in net.nnodes:
                i.append(node.pin.name)
            i.append(net.name)
            file.write(pfx + "\t".join(i) + "\n")

    def decide_cname(self):
        ''' Find name '''
        i = str(util.sortkey(self.nets[-1].cname)[-1]).rsplit(".", maxsplit=1)[-1]
        self.cname = self.nets[0].cname + "_to_" + i

    def register(self):
        ''' Register network on nodes '''
        self.decide_cname()
        self.sort_nets()
        for net in self.nets:
            net.netbus = self
            for node in net.nnodes:
                node.netbus = self
        net = self.nets[0]
        if net.sc_type != "bool":
            self.ctype = "_rv <%d>" % len(self.nets)
        elif len(self.nets) <= 16:
            self.ctype = "uint16_t"
        elif len(self.nets) <= 32:
            self.ctype = "uint32_t"
        else:
            self.ctype = "uint64_t"

    def write_decl(self, net, file):
        ''' Write network declaration '''
        if net == self.nets[0]:
            lname = self.cname.split(".")[-1]
            if net.sc_type != "bool":
                file.write("\tsc_signal_rv <%d> %s;\n" % (len(self.nets), lname))
            else:
                file.write("\tsc_signal <%s> %s;\n" % (self.ctype, lname))

    def write_init(self, net, file):
        ''' Write network initialization '''
        if net == self.nets[0]:
            lname = self.cname.split(".")[-1]
            if net.sc_type != "bool":
                file.write(',\n\t%s("%s", "%s")' % (lname, lname, "z" * len(self.nets)))
            else:
                file.write(',\n\t%s("%s", 0x%xULL)' % (lname, lname, (1 << len(self.nets)) - 1))

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
        ''' Determine if network needs hiz state '''
        for _gnam, net in sorted(self.cpu.nets.items()):
            if len(net) == 0:
                continue

            if net.is_supply:
                net.sc_type = "bool"
                continue

            hizs = 0
            outputs = 0
            roles = set()
            for node in net.iter_nodes():
                roles.add(node.pin.type)
                if node.pin.type.hiz:
                    hizs += 1
                if node.pin.type.output:
                    outputs += 1

            if outputs == 0:
                print("Undriven", net, len(net), roles)
                for i in net.nnodes:
                    print("  ", i)

            if hizs == 0 and outputs <= 1:
                net.sc_type = "bool"

    def bus_candidates(self, file):
        ''' Find networks which are candidates for bus-membership '''
        for _gnam, net in sorted(self.cpu.nets.items()):

            if len(net.nnodes) < 2:
                continue

            if None in (x.pin.pinbus for x in net.nnodes):
                continue

            # uniq_comps = set(x.component.gref for x in net.nnodes)
            uniq_comps = set((x.component.gref, x.pin.pinbus) for x in net.nnodes)
            if len(uniq_comps) != len(net.nnodes):
                # Signals which connect multiple times to a single component
                # are disqualified, because we cannot (easily) figure out
                # which of the multiple connections to assign where in the bus.
                #print("Mult", net)
                #print("  NN", len(net.nnodes))
                #print("  UC", uniq_comps)
                continue


            sig = " ".join(
                sorted(
                    ":".join((x.component.board.name, x.component.ref, x.pin.pinbus.name))
                    for x in net.nnodes
                )
            )
            i = self.netbusses.get(sig)
            if i:
                i.add_net(net)
            else:
                self.netbusses[sig] = NetBus(sig, net)

        for sig, maybebus in list(self.netbusses.items()):
            maybebus.sort_nets()
            if len(maybebus.nets) < MIN_BUS_WIDTH:
                del self.netbusses[sig]
                continue
            if maybebus.invalid(file):
                del self.netbusses[sig]
                continue

        file.write("\n")
        file.write("Residual busses\n")
        file.write("===============\n")
        accepted = []
        for sig, maybebus in self.netbusses.items():
            while len(maybebus.nets) >= MIN_BUS_WIDTH:
                maybebus.sort_nets()
                maybebus.best = []
                if not maybebus.unordered():
                    accepted.append(maybebus)
                    break
                if len(maybebus.best) < MIN_BUS_WIDTH:
                    break

                target = set(maybebus.best)
                rest = set(maybebus.nets) - target
                target = list(target)
                rest = list(rest)

                newbus = NetBus(maybebus.sig, target.pop(0))
                while target:
                    newbus.add_net(target.pop(0))
                accepted.append(newbus)

                maybebus = NetBus(maybebus.sig, rest.pop(0))
                while rest:
                    maybebus.add_net(rest.pop(0))
            if maybebus not in accepted:
                file.write("\n")
                maybebus.table(file)

        file.write("\n")
        file.write("Accepted busses\n")
        file.write("===============\n")
        for netbus in accepted:
            netbus.register()
            file.write("\n")
            netbus.table(file)
