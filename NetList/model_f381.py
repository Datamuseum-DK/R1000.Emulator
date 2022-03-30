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
   F381 4-bit Arithemtic Logic Unit
   ================================

   Ref: Fairchild DS009528 May 1988 Revised August 1999
'''


from part import PartModel, PartFactory

class F381(PartFactory):

    ''' F381 4-bit Arithemtic Logic Unit '''

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|
		|	unsigned a = 0, b = 0, c = 0, sel = 0, r, p = 1, g = 1;
		|
		|	if (PIN_A3=>) a |= 1;
		|	if (PIN_A2=>) a |= 2;
		|	if (PIN_A1=>) a |= 4;
		|	if (PIN_A0=>) a |= 8;
		|	if (PIN_D3=>) b |= 1;
		|	if (PIN_D2=>) b |= 2;
		|	if (PIN_D1=>) b |= 4;
		|	if (PIN_D0=>) b |= 8;
		|	if (PIN_S2=>) sel |= 1;
		|	if (PIN_S1=>) sel |= 2;
		|	if (PIN_S0=>) sel |= 4;
		|	if (PIN_CI=>) c |= 1;
		|
		|	switch (sel) {
		|	case 0: r = 0; break;
		|	case 1: r = 0xf + c + b - a; break;
		|	case 2: r = 0xf + c + a - b; break;
		|	case 3: r = a + b + c; break;
		|	case 4: r = a ^ b; break;
		|	case 5: r = a | b; break;
		|	case 6: r = a & b; break;
		|	case 7: r = 0xf; break;
		|	}
		|
		|	if (c && r == 0x10)
		|		p = 0;
		|	else if (r == 0xf && sel < 4)
		|		p = c;
		|	else if (r == 0xf)
		|		p = 0;
		|	else if (r > 0xf)
		|		g = 0;
		|
		|	PIN_F3<=(r & 1);
		|	PIN_F2<=(r & 2);
		|	PIN_F1<=(r & 4);
		|	PIN_F0<=(r & 8);
		|	PIN_G<=(g);
		|	PIN_P<=(p);
		|
		|	r &= 0xf;
		|
		|	TRACE(
		|	    << " a " << a
		|	    << " b " << b
		|	    << " c " << c
		|	    << " sel " << sel
		|	    << " r " << r
		|	    << " p " << p
		|	    << " g " << g
		|	);
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("F381", PartModel("F381", F381))
