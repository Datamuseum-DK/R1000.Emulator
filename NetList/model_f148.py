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
   F148 8-Line to 3-Line Priority Encoder
   ======================================

   Ref: Fairchild DS009480 April 1988 Revised September 2000
'''

from part import PartModel, PartFactory

class F148(PartFactory):

    ''' F148 8-Line to 3-Line Priority Encoder '''


    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|
		|	unsigned s;
		|
		|	if (!PIN_I0=>)
		|		s = 7;
		|	else if (!PIN_I1=>)
		|		s = 6;
		|	else if (!PIN_I2=>)
		|		s = 5;
		|	else if (!PIN_I3=>)
		|		s = 4;
		|	else if (!PIN_I4=>)
		|		s = 3;
		|	else if (!PIN_I5=>)
		|		s = 2;
		|	else if (!PIN_I6=>)
		|		s = 1;
		|	else if (!PIN_I7=>)
		|		s = 0;
		|	else
		|		s = 8|16;
		|	if (PIN_E=>)
		|		s = 16;
		|	TRACE(
		|	    << " i " << PIN_I0?
		|	    << PIN_I1?
		|	    << PIN_I2?
		|	    << PIN_I3?
		|	    << PIN_I4?
		|	    << PIN_I5?
		|	    << PIN_I6?
		|	    << PIN_I7?
		|	    << " e " << PIN_E?
		|	    << " | "
		|	    << std::hex << s
		|	);
		|	PIN_Y2<=(!(s & 1));
		|	PIN_Y1<=(!(s & 2));
		|	PIN_Y0<=(!(s & 4));
		|	PIN_GS<=(s & 16);
		|	PIN_EZ<=(!(s & 8));
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("F148", PartModel("F148", F148))
