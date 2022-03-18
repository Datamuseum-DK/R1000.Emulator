#!/usr/bin/env python3

'''
   Turn kicad netlist files into SystemC source code
   =================================================
'''

class Pin():
    ''' A `pin` on a `libpart` from the netlist file '''
    def __init__(self, libpart, sexp):
        self.libpart = libpart
        self.sexp = sexp

        self.num = sexp[0][0].name
        self.name = sexp[1][0].name
        self.type = sexp[2][0].name
        self.libpart.pins[self.num] = self

    def __str__(self):
        return "_".join((str(self.libpart), self.num, self.name, self.type))

    def __lt__(self, other):
        if self.num.isdigit() and other.num.isdigit():
            return int(self.num, 10) < int(other.num, 10)
        if self.num[0] == other.num[0]:
            return int(self.num[1:], 10) < int(other.num[1:], 10)
        return self.num < other.num
