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
   F521 8 bit comparator
   =====================
'''


from part import PartModel, PartFactory

class F521(PartFactory):

    ''' F521 8 bit comparator '''

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	bool s = PIN_E=> ||
		|	    PIN_A0=> != PIN_B0=> ||
		|	    PIN_A1=> != PIN_B1=> ||
		|	    PIN_A2=> != PIN_B2=> ||
		|	    PIN_A3=> != PIN_B3=> ||
		|	    PIN_A4=> != PIN_B4=> ||
		|	    PIN_A5=> != PIN_B5=> ||
		|	    PIN_A6=> != PIN_B6=> ||
		|	    PIN_A7=> != PIN_B7=>;
		|	TRACE(
		|	    << " Ia=b " << PIN_E?
		|	    << " a " << PIN_A0? << PIN_A1? << PIN_A2? << PIN_A3?
		|		<< PIN_A4? << PIN_A5? << PIN_A6? << PIN_A7?
		|
		|	    << " b " << PIN_B0? << PIN_B1? << PIN_B2? << PIN_B3?
		|		<< PIN_B4? << PIN_B5? << PIN_B6? << PIN_B7?
		|	    << " = "
		|	    << s
		|	);
		|	PIN_AeqB<=(s);
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("F521", PartModel("F521", F521))
