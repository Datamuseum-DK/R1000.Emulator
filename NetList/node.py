#!/usr/bin/env python3

'''
   Turn kicad netlist files into SystemC source code
   =================================================
'''

class Node():
    ''' A `node` from the netlist file '''
    def __init__(self, net, sexp):
        self.net = net
        self.sexp = sexp
        self.refname = sexp[0][0].name[1:-1]
        self.pinno = sexp[1][0].name[1:-1]
        self.pinfunction = sexp.find_first("pinfunction")
        if self.pinfunction:
            self.pinfunction = self.pinfunction[0].name
        self.component = self.net.board.components[self.refname]
        self.component.connections[self.pinno] = self

    def __str__(self):
        return "_".join((str(self.net), str(self.component), str(self.pinno)))
