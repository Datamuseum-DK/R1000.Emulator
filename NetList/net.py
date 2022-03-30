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

from node import NodeSexp

class Net():
    ''' A `net` from the netlist file '''
    def __init__(self, board, netname):
        self.board = board
        self.name = netname
        self.nodes = []
        self.bus = None
        self.sheets = set()
        self.is_plane = None
        self.is_local = None
        self.sc_type = "sc_logic"
        self.cname = None

    def add_node(self, node):
        ''' ... '''
        self.nodes.append(node)

    def iter_nodes(self):
        yield from self.nodes

    def find_cname(self):
        ''' This is gnarly '''
        self.cname = self.name
        if self.cname[0].isdigit():
            self.cname = "z" + self.cname
        if len(self.nodes) == 1 and "unconnected" in self.cname:
            node = self.nodes[0]
            self.cname = "u_" + node.component.name + "_" + node.pin.ident
            # print("ZZ", self.cname, self.nodes[0])
        for find, replace in (
            ("Net-", ""),
            ("/Page ", "p"),
            ("unconnected-", ""),
            ("(", ""),
            (")", ""),
            ("-", "_"),
            ("/", "_"),
            (".", "_"),
            ("~", "inv"),
        ):
            self.cname = self.cname.replace(find, replace)
        self.bcname = self.cname
        if self.is_plane:
            self.cname = "planes." + self.name
        elif not self.is_local:
            self.cname = self.board.lname + "_globals." + self.cname

    def __repr__(self):
        return "_".join(("Net", self.name))

    def __lt__(self, other):
        return self.name < other.name

    def __len__(self):
        return len(self.nodes)

    def is_pd(self):
        ''' Is this network a constant low value '''
        return self.name == "PD"

    def is_pu(self):
        ''' Is this network a constant high value '''
        return self.name == "PU"

    def is_const(self):
        return self.name in ("PD", "PU") or len(self) == 1

    def write_decl(self, file):
        ''' Write a C declaration of this net '''
        if self.bus:
            self.bus.write_decl(self, file)
        else:
            if self.sc_type == "bool":
                text = "\tsc_signal <bool> " + self.bcname + ";\t"
            else:
                text = "\tsc_signal_resolved " + self.bcname + ";\t"
            while len(text.expandtabs()) < 64:
                text += "\t"
            file.write(text + "// " + self.name + "\n")

    def write_init(self, file):
        ''' Write a C initialization of this net '''
        if self.bus:
            self.bus.write_init(self, file)
        elif self.sc_type == "bool":
            file.write(",\n\t" + self.bcname + '("' + self.bcname + '", true)')
        else:
            file.write(",\n\t" + self.bcname + '("' + self.bcname + '", sc_logic_1)')

    def ponder(self):
        ''' post-parsing work '''
        if self.name[:2] in ("GB", "GF", "PD", "PU",):
            return
        census = {}
        for node in self.nodes:
            census[node.pin.role] = 1 + census.get(node.pin.role, 0)
        if len(census) == 1 and 'output+no_connect' in census:
            return
        if len(census) == 1 and 'input+no_connect' in census:
            return
        if len(census) == 1 and 'tri_state+no_connect' in census:
            return
        if sum(census.values()) > 1 and 'tri_state' in census:
            return
        if 'input' in census and 'tri_state' in census:
            return
        if 'input' in census and 'output' in census:
            return
        print("    ", self, census)

class NetSexp(Net):
    ''' ... '''

    def __init__(self, board, sexp):
        super().__init__(
            board,
            netname = sexp[1][0].name,
        )
        for node_sexp in sexp.find("node"):
            node = NodeSexp(self, node_sexp)
            self.add_node(node)
