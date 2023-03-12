#!/usr/local/bin/python3
#
# Copyright (c) 2023 Poul-Henning Kamp
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
   Multiplexers to replace HiZ busses
   ==================================
'''

from part import PartModel, PartFactory
from component import Component

class XBusMux(PartFactory):

    ''' HiZ Bus Multiplexer '''

    def __init__(self, board, ident, length, width):
        super().__init__(board, ident)
        self.length = length
        self.width = width

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        for x in self.comp.nodes.items():
            file.write("// " + str(x) + "\n")

        file.fmt('''
		|	uint64_t tmp;
		|	const char *which;
		|
		|''')
        pfx = ""
        for i in range(self.width):
            file.fmt('\t' + pfx + 'if (!PIN_OE%c) {\n' % (i + 65))
            pfx = "} else "
            file.fmt('\t\twhich = "%c";\n' % (i + 65))
            file.fmt('\t\tBUS_I%c_READ(tmp);\n' % (i + 65))
            file.fmt('\t\tBUS_Q_WRITE(tmp);\n')

        file.fmt('''
		|	} else {
		|		which = "?";
		|		tmp = 0;
		|	}
		|	TRACE(
		|''')

        for i in range(self.width):
            file.fmt('\t    << " %c " << PIN_OE%c? << BUS_I%c_TRACE()\n' % (i + 97, i+65, i+65))

        file.fmt('''
		|	    << " - " << std::hex << tmp
		|	);
		|''')


class ModelXBusMux(PartModel):
    ''' HiZ bus mux '''

    def __init__(self, length, width):
        super().__init__("XBUSMUX%dX%d" % (length, width))
        self.length = length
        self.width = width

    def configure(self, board, comp):
        sig = self.make_signature(comp)
        ident = self.name + "_" + sig
        if ident not in board.part_catalog:
            board.add_part(ident, XBusMux(board, ident, self.length, self.width))
        comp.part = board.part_catalog[ident]

def register(board):
    ''' Register component model '''

    for i, j in (
        (4, 2),
        (4, 4),
        (5, 2),
        (5, 5),
        (7, 3),
        (7, 4),
        (8, 2),
        (8, 3),
        (8, 5),
        (9, 2),
        (10, 2),
        (11, 2),
        (13, 4),
        (15, 2),
        (15, 6),
        (16, 2),
        (16, 4),
        (16, 6),
        (19, 2),
        (20, 2),
        (20, 4),
        (31, 2),
        (32, 2),
    ):
        board.add_part("XBUSMUX%dX%d" % (i, j), ModelXBusMux(i, j))
