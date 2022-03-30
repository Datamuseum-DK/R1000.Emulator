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
   F283 Binary Full Adder with Fast Carry
   ======================================

   Ref: Fairchild DS009513 April 1988 Revised January 2004
'''

from part import PartModel, PartFactory

class F283(PartFactory):

    ''' F283 Binary Full Adder with Fast Carry '''

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|
		|	unsigned sum = 0;
		|
		|	if (PIN_CI=>) sum += 1;
		|	if (PIN_A3=>) sum += 1;
		|	if (PIN_B3=>) sum += 1;
		|	if (PIN_A2=>) sum += 2;
		|	if (PIN_B2=>) sum += 2;
		|	if (PIN_A1=>) sum += 4;
		|	if (PIN_B1=>) sum += 4;
		|	if (PIN_A0=>) sum += 8;
		|	if (PIN_B0=>) sum += 8;
		|	TRACE(
		|	    << " a " << PIN_A0? << PIN_A1? << PIN_A2? << PIN_A3?
		|	    << " b " << PIN_B0? << PIN_B1? << PIN_B2? << PIN_B3?
		|	    << " ci " << PIN_CI?
		|	    << " sum "
		|	    << std::hex << sum
		|	);
		|	PIN_Y3<=(sum & 1);
		|	PIN_Y2<=(sum & 2);
		|	PIN_Y1<=(sum & 4);
		|	PIN_Y0<=(sum & 8);
		|	PIN_C0<=(sum & 16);
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("F283", PartModel("F283", F283))
