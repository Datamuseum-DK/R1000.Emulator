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
   Busses of multiple nets
   =======================
'''

MIN_BUS_WIDTH = 4
MIN_BUS_MEMBERS = 3

SHOW_VETOED_BUSSES = 0

class BusAttach():
    ''' Describes how a component can attach to a bus '''

    def __init__(self, comp, bus, numeric=False, order=None, output=False):
        self.comp = comp
        self.bus = bus
        self.numeric = numeric
        self.order = order
        self.output = output

    def __repr__(self):
        text = "<BusAttach %s %s" % (self.comp.name, self.bus.busname) + self.show() + ">"
        return text

    def show(self):
        text = ""
        if self.output:
            text += "W"
        if self.numeric:
            text += "#"
        if self.order:
            text += "…"
        return text

class Bus():

    ''' A group of nets which reach the same set of components '''

    def __init__(self):
        self.busname = None
        self.cname = None
        self.nets = []
        self.components = []
        self.filtered = []
        self.good = False
        self.numeric = False

    def add_net(self, net):
        ''' ... '''
        if len(net.nodes) < MIN_BUS_MEMBERS:
            return
        self.nets.append(net)
        if len(self.nets) == 1:
            for node in net.nodes:
                self.components.append(node.component)

    def __len__(self):
        return len(self.nets)

    def __repr__(self):
        return "<Bus %d*%d %s>" % (len(self.nets[0].nodes), len(self), self.busname)

    def mynodes(self, comp):
        ''' Iterate the nodes, in order, belonging to `comp` '''
        for net in self.nets:
            for node in net.nodes:
                if node.component == comp:
                    yield node
                    break

    def filter(self):
        ''' Filter out busses which components dont like '''

        # self.nets.sort(key=lambda net: net.nodes[0].pinfunction)
        self.components.sort()

        self.find_name()

        refused = []
        for comp in self.components:
            i = comp.is_bus_ok(self)
            if not i:
                refused.append(comp)
            else:
                self.filtered.append(i)

        if refused:
            comps = set(x.partname for x in refused)
            if SHOW_VETOED_BUSSES > 1:
                print()
            if SHOW_VETOED_BUSSES:
                print("Vetoed by", len(comps), ", ".join(sorted(comps)), ":", self)
            if SHOW_VETOED_BUSSES > 1:
                self.table()
            return

        orders = list(x.order for x in self.filtered if x.order)
        if len(orders) > 0:
            for i in zip(*orders):
                if len(set(node.net for node in i)) != 1:
                    self.table()
                    print("Disagreement about bus-order:")
                    for j in self.filtered:
                        print("    ", j.comp.name, list(x.pinfunction for x in j.order))
                    return
            self.nets = [node.net for node in orders[0]]

        self.numeric = min(x.numeric for x in self.filtered)
        outputs = 0
        for i in self.filtered:
            if i.output:
                outputs += 1
        if outputs > 1:
            self.numeric = False

        self.table(self.filtered)
        print("Numeric", self.numeric)
        print("Outputs", outputs)
        print("Good bus", self.busname, ", ".join(sorted(set(x.partname for x in self.components))))

        self.good = True

        for comp in self.components:
            comp.commit_bus(self)

        for net in self.nets:
            net.bus = self

    def find_name(self):
        ''' Derive a good name for this bus '''
        names = [x.cname for x in self.nets]

        if names[0][0] == 'U':
            # Anonymous busses have signals named 'U6416_Pad7'
            # Translate those to that chips pinfunctions
            for comp in self.components:
                if comp.ref == names[0][:len(comp.ref)]:
                    names = []
                    for node in self.mynodes(comp):
                        names.append(comp.name + "_" + node.pinfunction)
                    break

        for cut in range(len(names[0])):
            if len(set(x[cut] for x in names)) > 1:
                break
        sfxs = []
        nbrs = []
        for j in names:
            sfxs.append(j[cut:])
            try:
                nbrs.append(int(sfxs[-1], 10))
            except ValueError:
                nbrs.append(-1)

        self.busname = names[0][:cut]
        if min(nbrs) > -1 and len(nbrs) == 1 + max(nbrs) - min(nbrs):
            self.busname += str(min(nbrs)) + "__" + str(max(nbrs))
        else:
            if cut > 0 and self.busname[-1] != '_':
                self.busname += '_'
            self.busname += "_".join(sfxs)

        self.cname = self.busname.split('.')[-1]

    def table(self, filtered=None):
        ''' Print bus as a table '''
        text = str(self)
        print("-" * len(text))
        print(text)
        print("-" * len(text))
        line = []
        for comp in self.components:
            line.append(comp.partname)
        print("\t".join(line))

        line = []
        for comp in self.components:
            line.append(comp.name)
        print("\t".join(line))

        line = []
        for comp in self.components:
            line.append(comp.ref)
        print("\t".join(line))

        if filtered:
            line = []
            for filter in self.filtered:
                line.append(filter.show())
            print("\t".join(line))

        for net in self.nets:
            line = []
            for comp in self.components:
                for node in net.nodes:
                    if node.component == comp:
                        if node.pinfunction:
                            line.append(node.pinfunction)
                        else:
                            line.append("?")
                        break
            line.append(net.cname)
            print("\t".join(line))

    def write_decl(self, net, file):
        ''' ... '''
        if net != self.nets[0]:
            return
        if self.numeric:
            file.write('\tsc_signal <uint64_t> %s;\n' % self.cname)
        else:
            file.write('\tsc_signal_rv < %d > %s;\n' % (len(self.nets), self.cname))

    def write_init(self, net, file):
        ''' ... '''
        if net == self.nets[0]:
            file.write(',\n\t%s ("%s")' % (self.cname, self.cname))

class BusSchedule():
    ''' ... '''
    def __init__(self, board):
        self.board = board
        self.busses = {}

        for net in self.board.nets.values():
            if not net.busable:
                continue
            key = (node.component.name for node in net.nodes)
            key = ",".join(sorted(key))
            bus = self.busses.get(key)
            if not bus:
                bus = self.busses.setdefault(key, Bus())
            bus.add_net(net)
        for key in list(self.busses):
            if len(self.busses[key]) < MIN_BUS_WIDTH:
                del self.busses[key]
        for bus in self.busses.values():
            bus.filter()
        print(len(self.busses), "potential busses")
        print(len([x for x in self.busses.values() if x.good]), "good busses")
