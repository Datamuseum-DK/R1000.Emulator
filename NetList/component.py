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
   Turn kicad netlist files into SystemC source code
   =================================================
'''

import pin

class Component():
    ''' A `component` from the netlist file '''


    def __init__(self, board, compref, compvalue, compsheet, comppart):
        self.board = board
        self.ref = compref
        self.value = compvalue
        self.nodes = {}
        self.busses = []
        self.model = None
        self.sheet = compsheet
        self.partname = comppart
        self.partinfo = None
        self.location = "x99"
        self.name = "X"
        self.part = None
        self.busses = {}
        self.is_plane = self.partname in ("GF", "GB")
        self.is_supply = self.partname in ("PU", "PD", "Pull_Down", "Pull_Up")

        self.insert()

    def __str__(self):
        return "_".join((str(self.sheet), self.ref, self.partname, self.location, self.name))

    def __lt__(self, other):
        return self.name < other.name

    def __contains__(self, idx):
        return self.nodes.__contains__(idx)

    def __getitem__(self, idx):
        return self.nodes.__getitem__(idx)

    def __iter__(self):
        yield from list(sorted(self.nodes.values()))

    def insert(self):
        ''' Insert this component '''
        self.board.components[self.ref] = self
        self.sheet.add_component(self)

    def remove(self):
        ''' Remove this component '''
        del self.board.components[self.ref]
        self.sheet.del_component(self)

    def add_node(self, node):
        ''' Add a node to this component '''
        assert node.pin.name not in self.nodes
        self.nodes[node.pin.name] = node

    def del_node(self, node):
        ''' Remove a node to this component '''
        assert node.pin.name in self.nodes
        del self.nodes[node.pin.name]

    def make_busses(self):
        ''' Deduce busses from nodes and pins '''
        buscand = {}
        for node in self:
            sortkey = node.pin.sortkey
            if len(sortkey) == 2:
                buscand.setdefault(sortkey[0], list()).append(node)
        for busname, busnodes in buscand.items():
            if not busname.isalpha():
                continue
            if len(busnodes) < 2:
                continue
            low = min(x.pin.sortkey[1] for x in busnodes)
            high = max(x.pin.sortkey[1] for x in busnodes)
            if 1 + high - low != len(busnodes):
                continue
            bus = pin.PinBus(busname, low)
            self.busses[busname] = bus
            for node in busnodes:
                bus.add_pin(node.pin)

class ComponentSexp(Component):

    ''' Create a `component` from netlist-sexp '''

    def __init__(self, board, sexp):
        i = sexp.find_first("sheetpath.names")
        sheet = board.sheets[board.pagename_to_sheet(i[0].name)]
        i = sexp.find_first("libsource.part")
        partname = i[0].name

        super().__init__(
            board,
            compref = sexp[0][0].name,
            compvalue = sexp.find_first("value")[0].name,
            compsheet = sheet,
            comppart = partname,
        )

        for i in sexp.find("property"):
            if i[0][0].name == 'Location':
                self.location = i[1][0].name
            elif i[0][0].name == 'Name':
                self.name = i[1][0].name
