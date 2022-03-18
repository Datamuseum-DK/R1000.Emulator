#!/usr/bin/env python3

'''
   Busses of multiple nets
   =======================
'''

class Bus():

    ''' A group of nets which reach the same set of components '''

    def __init__(self):
        self.busname = None
        self.nets = []
        self.components = []

    def add_net(self, net):
        ''' ... '''
        self.nets.append(net)
        if len(self.nets) == 1:
            for node in net.nodes:
                self.components.append(node.component)

    def __len__(self):
        return len(self.nets)

    def filter(self):
        self.find_name()
        self.table()

    def find_name(self):
        ''' Derive a good name for this bus '''
        names = [x.cname for x in self.nets]
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
        if self.busname[-1] != '_':
            self.busname += '_'
        if min(nbrs) > -1 and len(nbrs) == 1 + max(nbrs) - min(nbrs):
            self.busname += str(min(nbrs)) + "__" + str(max(nbrs))
        else:
            self.busname += "_".join(sfxs)

    def table(self):
        print()
        print("-" * len(self.busname))
        print(self.busname)
        print("-" * len(self.busname))
        line = []
        for comp in sorted(self.components):
            line.append(comp.partname)
        print("\t".join(line))
        line = []
        for comp in sorted(self.components):
            line.append(comp.name)
        print("\t".join(line))

        for net in sorted(self.nets):
            line = []
            for comp in sorted(self.components):
                for node in net.nodes:
                    if node.component == comp:
                        line.append(node.pinfunction)
                        break
            line.append(str(net))
            line.append(net.cname)
            print("\t".join(line))

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
            if len(self.busses[key]) < 2:
                del self.busses[key]
        print(len(self.busses), "potential busses")
        for bus in self.busses.values():
            bus.filter()
