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
   F139 (Dual) 1-of-4 Decoder/Demultiplexer
   ========================================

   Ref: Fairchild DS009479 April 1988 Revised September 2000
'''


from part import PartModel, PartFactory

class F139(PartFactory):

    ''' F139 (Dual) 1-of-4 Decoder/Demultiplexer '''

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	unsigned adr = 0;
		|
		|	if (PIN_B1=>) adr |= 1;
		|	if (PIN_B0=>) adr |= 2;
		|	if (PIN_E=>) adr |= 4;
		|	TRACE(
		|	    << "e " << PIN_E?
		|	    << " i " << PIN_B0? << PIN_B1?
		|	    << " o " << adr
		|	);
		|	PIN_Y0<=(adr != 0);
		|	PIN_Y1<=(adr != 1);
		|	PIN_Y2<=(adr != 2);
		|	PIN_Y3<=(adr != 3);
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("F139", PartModel("F139", F139))
