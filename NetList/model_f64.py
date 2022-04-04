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

    def assign(self, comp):

        and1 = Component(
            board = comp.board,
            compref = comp.ref + "_1",
            compvalue = comp.value,
            compsheet = comp.sheet,
            comppart = "AND2",
        )
        and1.name = comp.name + "_1"
        and2 = Component(
            board = comp.board,
            compref = comp.ref + "_2",
            compvalue = comp.value,
            compsheet = comp.sheet,
            comppart = "AND2",
        )
        and2.name = comp.name + "_2"
        and3 = Component(
            board = comp.board,
            compref = comp.ref + "_3",
            compvalue = comp.value,
            compsheet = comp.sheet,
            comppart = "AND3",
        )
        and3.name = comp.name + "_3"
        and4 = Component(
            board = comp.board,
            compref = comp.ref + "_4",
            compvalue = comp.value,
            compsheet = comp.sheet,
            comppart = "AND4",
        )
        and4.name = comp.name + "_4"
        nor = Component(
            board = comp.board,
            compref = comp.ref + "_5",
            compvalue = comp.value,
            compsheet = comp.sheet,
            comppart = "NOR4",
        )
        nor.name = comp.name + "_5"

        for fmname, tocomp, toname in (
            ("A0", and1, "D0"),
            ("A1", and1, "D1"),
            ("B0", and2, "D0"),
            ("B1", and2, "D1"),
            ("C0", and3, "D0"),
            ("C1", and3, "D1"),
            ("C2", and3, "D2"),
            ("D0", and4, "D0"),
            ("D1", and4, "D1"),
            ("D2", and4, "D2"),
            ("D3", and4, "D3"),
            ("Q", nor, "Q"),
        ):
            node = comp[fmname]
            node.remove()
            node.pin.name = toname
            node.component = tocomp
            node.insert()

        for andcomp, netsuf, norpin in (
            (and1, "_0", "D0"),
            (and2, "_1", "D1"),
            (and3, "_2", "D2"),
            (and4, "_3", "D3"),
        ):
            net = Net(comp.board, "AOI_" + comp.name + netsuf)
            pin1 = Pin(
                "q",
                "Q",
                "c_output",
            )
            pin2 = Pin(
                "d",
                norpin,
                "c_input",
            )
            Node(net, andcomp, pin1)
            Node(net, nor, pin2)

        for comp2 in (and1, and2, and3, and4, nor):
            comp2.part = comp.board.part_catalog[comp2.partname]
            comp2.part.assign(comp2)

        for node in comp:
            node.remove()
        comp.remove()

def register(board):
    ''' Register component model '''

    board.add_part("F64", ModelAOI("AOI"))
