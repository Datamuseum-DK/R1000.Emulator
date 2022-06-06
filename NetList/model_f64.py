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
   F24[04] Octal buffers (3-state)
   ===============================

   Ref: Philips IC15 1994 Dec 05
'''

from pin import Pin
from node import Node
from part import PartModel
from component import Component
from net import Net

class ModelAOI(PartModel):
    ''' And-Or-Invert '''

    def __init__(self, partname, inputs):
        super().__init__(partname)
        self.inputs = inputs
        self.ctr = 0

    def and_input(self, comp, inputs):
        ''' Do one of the input AND gates '''
        nodes = []
        nets = set()
        for pinname in inputs:
            node = comp[pinname]
            if node.net.is_pd():
                return list()
            if node.net.is_pu():
                continue
            if node.net in nets:
                continue
            nodes.append(node)
            nets.add(node.net)
        assert len(nodes) > 0
        if len(nodes) < 2:
            return nodes
        self.ctr += 1
        and_gate = Component(
            board = comp.board,
            compref = comp.ref + "_AND_%d" % self.ctr,
            compvalue = comp.value,
            compsheet = comp.sheet,
            comppart = "AND%d" % len(nodes),
        )
        and_gate.name = comp.name + "_%d" % self.ctr
        and_gate.part = comp.board.part_catalog[and_gate.partname]

        net = Net(comp.board, self.name + "_" + comp.name + "_%d" % self.ctr)
        for pnum, node in enumerate(nodes):
            pin = Pin("d", "D%d" % pnum, "c_input")
            node = Node(node.net, and_gate, pin)

        pin = Pin("q", "Q", "c_output")
        node = Node(net, and_gate, pin)
        and_gate.part.assign(and_gate)
        return [ node ]

    def assign(self, comp):
        self.ctr = 0
        nor_nodes = []
        for i in self.inputs:
            nor_nodes += self.and_input(comp, i)

        nor = Component(
            board = comp.board,
            compref = comp.ref + "_NOR",
            compvalue = comp.value,
            compsheet = comp.sheet,
            comppart = "NOR%d" % len(nor_nodes),
        )
        nor.name = comp.name + "_NOR"

        for dnum, node in enumerate(nor_nodes):
            net = node.net
            pin = Pin(
                "d",
                "D%d" % dnum,
                "c_input",
            )
            Node(net, nor, pin)

        pin = Pin("q", "Q", "c_output")
        node = Node(comp["Q"].net, nor, pin)

        nor.part = comp.board.part_catalog[nor.partname]
        nor.part.assign(nor)

        for node in comp:
            node.remove()
        comp.remove()

def register(board):
    ''' Register component model '''

    board.add_part(
        "F51",
        ModelAOI(
            "F51",
            (
                 ("A1", "A2", "A3"),
                 ("B1", "B2", "B3"),
            )
        )
    )
    board.add_part(
        "F64",
        ModelAOI(
            "F64",
            (
                 ("A0", "A1"),
                 ("B0", "B1"),
                 ("C0", "C1", "C2"),
                 ("D0", "D1", "D2", "D3"),
            )
        )
    )
