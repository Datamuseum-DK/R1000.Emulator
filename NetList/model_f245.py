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
   F245 Octal bidirectional buffers
   ==========================================
'''

from part import PartModel, PartFactory
from component import Component
from node import Node
from pin import Pin

class F245(PartFactory):
    ''' ... '''

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        dir_ab = "AB_" in self.name

        file.fmt('''
		|       uint64_t tmp;
		|
		|	TRACE(
		|	    << " oe " << PIN_OE?
		|	    << " dir " << PIN_DIR?
		|	    << " a " << BUS_A_TRACE()
		|	);
		|''')

        if dir_ab:
            file.fmt('''
		|	if (!(PIN_OE=>) && (PIN_DIR=>)) {
		|''')
        else:
            file.fmt('''
		|	if (!(PIN_OE=>) && !PIN_DIR=>) {
		|''')

        file.fmt('''
		|		TRACE(
		|		    << " oe " << PIN_OE?
		|		    << " dir " << PIN_DIR?
		|		    << " a " << BUS_A_TRACE()
		|		);
		|		BUS_A_READ(tmp);
		|		BUS_Q_WRITE(tmp);
		|	} else {
		|		TRACE("Z");
		|		BUS_Q_Z();
		|		next_trigger(
		|''')

        if "OE" in self.comp and not self.comp["OE"].net.is_const():
            file.fmt('''
		|			PIN_OE.negedge_event() |
		|''')

        if dir_ab:
            file.fmt('''
		|			PIN_DIR.posedge_event()
		|''')
        else:
            file.fmt('''
		|			PIN_DIR.negedge_event()
		|''')

        file.fmt('''
		|		);
		|	}
		|''')

class ModelF245(PartModel):
    ''' F245 bidirectional buffers'''

    def assign(self, comp):
        ''' Split into two separate components '''
        for suff in ("AB", "BA"):
            if comp.nodes["OE"].net.is_pu():
                continue
            new_comp = Component(
                board = comp.board,
                compref = comp.ref + suff,
                compvalue = comp.value,
                compsheet = comp.sheet,
                comppart = comp.partname + suff
            )
            new_comp.name = comp.name + suff
            new_comp.part = comp.board.part_catalog[new_comp.partname]
            new_comp.part.assign(new_comp)
            for node in comp.nodes.values():
                if suff[1] == node.pin.name[0]:
                    node.net.no_bool = True
                    new_pin = Pin(
                        "Q" + node.pin.ident[1:],
                        "Q" + node.pin.name[1:],
                        "c_output",
                    )
                elif suff[0] == node.pin.name[0]:
                    new_pin = Pin(
                        "A" + node.pin.ident[1:],
                        "A" + node.pin.name[1:],
                        "c_input",
                    )
                else:
                    new_pin = Pin(
                        node.pin.ident,
                        node.pin.name,
                        "c_input",
                    )
                new_node = Node(
                    node.net,
                    new_comp, 
                    new_pin, 
                )
        for node in comp:
            node.net.sc_type = "sc_logic"
            node.remove()
        comp.remove()

def register(board):
    ''' Register component model '''

    for part in (
        "F245",
        "XBIDIR11",
        "XBIDIR16",
        "XBIDIR32",
        "XBIDIR64",
    ):
        board.add_part(part,        ModelF245(part, F245))
        board.add_part(part + "AB", PartModel(part + "AB", F245))
        board.add_part(part + "BA", PartModel(part + "BA", F245))