#!/usr/bin/env python3

'''
   Turn kicad netlist files into SystemC source code
   =================================================
'''

from pin import Pin

class LibPart():
    ''' A `libpart` from the netlist file '''
    def __init__(self, board, sexp):
        self.board = board
        self.sexp = sexp
        i = self.sexp.find_first("part")
        self.partname = i[0].name
        self.board.libparts[self.partname] = self
        self.pins = {}
        for i in self.sexp.find("pins.pin"):
            Pin(self, i)

    def __str__(self):
        return "_".join((str(self.board), self.partname))

    def include_file(self):
        ''' Return include statement for this libpart '''
        return 'Components/' + self.partname + '.hh'
