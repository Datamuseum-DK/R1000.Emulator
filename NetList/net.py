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

import util
from node import NodeSexp
from scmod import ScSignal

class Net():
    ''' A `net` from the netlist file '''
    def __init__(self, board, netname):
        self.board = board
        self.name = netname
        for i, j in (
            (">", "gt"),
            ("<", "lt"),
            ("=", "eq"),
            ("~", "not"),
        ):
            self.name = self.name.replace(i, j)
        self.sortkey = util.sortkey(self.name)
        self.nnodes = []
        self.netbus = None
        self.sheets = set()
        self.is_plane = None
        self.is_local = None
        self.is_supply = netname in ("GND", "+5V", "PU", "PD")
        if self.is_supply:
            self.sc_type = "bool"
        else:
            self.sc_type = "sc_logic"
        self.cname = None
        self.bcname = None
        self.default = True

        self.insert()

    def insert(self):
        ''' ... '''
        self.board.add_net(self)

    def remove(self):
        ''' ... '''
        self.board.del_net(self)

    def add_node(self, node):
        ''' ... '''
        assert node not in self.nnodes
        self.nnodes.append(node)

    def del_node(self, node):
        ''' ... '''
        assert node in self.nnodes
        self.nnodes.remove(node)
        if self.netbus:
            self.netbus.remove_node(node)

    def adopt(self, other):
        ''' Adopt another network '''
        for node in list(other.iter_nodes()):
            node.remove()
            node.net = self
            node.insert()
        if not other.is_supply:
            other.remove()

    def iter_nodes(self):
        ''' ... '''
        yield from self.nnodes

    def find_cname(self):
        ''' This is gnarly '''
        if self.is_pd():
            self.bcname = "PD"
            self.cname = "planes->PD"
            return
        if self.is_pu():
            self.bcname = "PU"
            self.cname = "planes->PU"
            return
        self.cname = self.name
        if self.cname[0].isdigit():
            self.cname = "z" + self.cname
        if len(self.nnodes) == 1 and "unconnected" in self.cname:
            node = self.nnodes[0]
            self.cname = "u_" + node.component.name + "_" + node.pin.ident
            # print("ZZ", self.cname, self.nnodes[0])
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
        if self.is_supply:
            self.cname = "///supply-c///"
        elif self.is_plane:
            self.cname = "planes->" + self.cname
        elif not self.is_local:
            self.cname = self.board.lname + "_globals->" + self.cname

    def __repr__(self):
        return "_".join(("Net", self.name))

    def __lt__(self, other):
        try:
            return self.sortkey < other.sortkey
        except:
            pass
        return str(self.sortkey) < str(other.sortkey)

    def __len__(self):
        return len(self.nnodes)

    def is_pd(self):
        ''' Is this network a constant low value '''
        return self.name == "PD"

    def is_pu(self):
        ''' Is this network a constant high value '''
        return self.name == "PU"

    def is_const(self):
        ''' Value will not change '''
        return self.name in ("PD", "PU") or len(self) == 1

    def sc_signals(self):
        ''' enumerate our SystemC signals '''
        if self.netbus:
            yield from self.netbus.sc_signals(self)
        else:
            retval = [self.bcname]
            if self.sc_type == "bool":
                retval.append("sc_signal <bool>")
            else:
                retval.append("sc_signal_resolved")
            if self.sc_type == "bool" and self.default:
                retval.append("true")
            elif self.sc_type == "bool":
                retval.append("false")
            elif self.default:
                retval.append("sc_logic_1")
            else:
                retval.append("sc_logic_1")
            yield ScSignal(*retval)

class NetSexp(Net):
    ''' ... '''

    def __init__(self, board, sexp):
        super().__init__(
            board,
            netname = sexp[1][0].name,
        )
        for node_sexp in sexp.find("node"):
            NodeSexp(self, node_sexp)
