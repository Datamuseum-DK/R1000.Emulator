#!/usr/bin/env python3

'''
   Turn kicad netlist files into SystemC source code
   =================================================
'''

from node import Node

class Net():
    ''' A `net` from the netlist file '''
    def __init__(self, board, sexp):
        self.board = board
        self.sexp = sexp
        self.name = self.sexp[1][0].name[1:-1]
        self.board.nets[self.name] = self
        self.nodes = []
        for i in sexp.find("node"):
            self.nodes.append(Node(self, i))
        self.sheets = set()
        self.is_plane = self.name in ("PU", "PD")
        self.busable = len(self.nodes) >= 4 and not self.is_plane
        for node in self.nodes:
            if node.component.is_plane:
                self.is_plane = True
                self.name = node.component.ref
                continue
            self.busable &= node.component.busable
            self.sheets.add(node.component.sheet)
            node.component.add_node(node)
        self.sheets = list(self.sheets)
        self.is_local = not self.is_plane and len(self.sheets) == 1
        if self.is_local:
            self.sheets[0].local_nets.append(self)
        self.find_cname()

        self.ponder()

    def find_cname(self):
        ''' This is gnarly '''
        self.cname = self.name
        if self.cname[0].isdigit():
            self.cname = "z" + self.cname
        if len(self.nodes) == 1 and "unconnected" in self.cname:
            i = self.nodes[0]
            self.cname = "u_" + i.component.name + "_" + i.pinno
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
        return self.name

    def __lt__(self, other):
        return self.name < other.name

    def decl(self):
        ''' Return a C declaration of this net '''
        text = "\tsc_signal_resolved " + self.bcname + ";\t"
        while len(text.expandtabs()) < 64:
            text += "\t"
        return text + "// " + self.name + "\n"

    def ponder(self):
        ''' post-parsing work '''
        if self.name[:2] in ("GB", "GF", "PD", "PU",):
            return
        census = {}
        for i in self.nodes:
            j = str(i.sexp.find_first("pintype")[0])
            census[j] = 1 + census.get(j, 0)
        if len(census) == 1 and '"output+no_connect"' in census:
            return
        if len(census) == 1 and '"input+no_connect"' in census:
            return
        if len(census) == 1 and '"tri_state+no_connect"' in census:
            return
        if sum(census.values()) > 1 and '"tri_state"' in census:
            return
        if '"input"' in census and '"tri_state"' in census:
            return
        if '"input"' in census and '"output"' in census:
            return
        print("    ", self, census)
