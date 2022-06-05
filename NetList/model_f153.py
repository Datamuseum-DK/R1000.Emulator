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
   F153 Dual 4-Input Multiplexer
   =============================

   Ref: Fairchild DS009482 April 1988 Revised September 2000
'''


from part import PartModel, PartFactory

class F153(PartFactory):

    ''' F153 Dual 4-Input Multiplexer '''

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	bool s[2];
		|	uint tmp = 0;
		|
		|	if (PIN_S0=>) tmp |= 2;
		|	if (PIN_S1=>) tmp |= 1;
		|
		|	switch (tmp) {
		|	case 0:
		|		s[0] = PIN_A0=>;
		|		s[1] = PIN_A1=>;
		|		break;
		|	case 1:
		|		s[0] = PIN_B0=>;
		|		s[1] = PIN_B1=>;
		|		break;
		|	case 2:
		|		s[0] = PIN_C0=>;
		|		s[1] = PIN_C1=>;
		|		break;
		|	case 3:
		|		s[0] = PIN_D0=>;
		|		s[1] = PIN_D1=>;
		|		break;
		|	}
		|	if (PIN_E0=>)
		|		s[0] = false;
		|	if (PIN_E1=>)
		|		s[1] = false;
		|	TRACE(
		|	    << " a " << PIN_A0? << PIN_A1?
		|	    << " b " << PIN_B0? << PIN_B1?
		|	    << " c " << PIN_C0? << PIN_C1?
		|	    << " d " << PIN_D0? << PIN_D1?
		|	    << " e " << PIN_E0? << PIN_E1?
		|	    << " s " << PIN_S0? << PIN_S1?
		|	    << " | " << s[0] << s[1]
		|	);
		|	PIN_Y0<=(s[0]);
		|	PIN_Y1<=(s[1]);
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("F153", PartModel("F153", F153, busable=False))
