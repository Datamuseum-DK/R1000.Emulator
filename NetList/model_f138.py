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
   F138 1-of-8 decoder/demultiplexer
   =================================

   Ref: Philips IC15 1991 Feb 14
'''


from part import PartModel, PartFactory

class F138(PartFactory):

    ''' F138 1-of-8 decoder/demultiplexer '''

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	unsigned adr, ena;
		|
		|	BUS_S_READ(adr);
		|	BUS_D_READ(ena);
		|	if (ena != 4)
		|		adr |= 8;
		|	TRACE(
		|	    << " s "
		|	    << BUS_S_TRACE()
		|	    << " e "
		|	    << BUS_D_TRACE()
		|	    << " | "
		|	    << std::hex << adr
		|	);
		|	PIN_Y7_<=(adr != 7);
		|	PIN_Y6_<=(adr != 6);
		|	PIN_Y5_<=(adr != 5);
		|	PIN_Y4_<=(adr != 4);
		|	PIN_Y3_<=(adr != 3);
		|	PIN_Y2_<=(adr != 2);
		|	PIN_Y1_<=(adr != 1);
		|	PIN_Y0_<=(adr != 0);
		|''')

class F154(PartFactory):

    ''' F138 1-of-8 decoder/demultiplexer '''

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	unsigned adr;
		|
		|	BUS_S_READ(adr);
		|	if (PIN_E1=> || PIN_E2=>)
		|		adr |= 16;
		|	TRACE(
		|	    << " s "
		|	    << BUS_S_TRACE()
		|	    << " e1 "
		|	    << PIN_E1?
		|	    << " e2 "
		|	    << PIN_E2?
		|	    << " | "
		|	    << std::hex << adr
		|	);
		|	PIN_Y15<=(adr != 15);
		|	PIN_Y14<=(adr != 14);
		|	PIN_Y13<=(adr != 13);
		|	PIN_Y12<=(adr != 12);
		|	PIN_Y11<=(adr != 11);
		|	PIN_Y10<=(adr != 10);
		|	PIN_Y9<=(adr != 9);
		|	PIN_Y8<=(adr != 8);
		|	PIN_Y7<=(adr != 7);
		|	PIN_Y6<=(adr != 6);
		|	PIN_Y5<=(adr != 5);
		|	PIN_Y4<=(adr != 4);
		|	PIN_Y3<=(adr != 3);
		|	PIN_Y2<=(adr != 2);
		|	PIN_Y1<=(adr != 1);
		|	PIN_Y0<=(adr != 0);
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("F138", PartModel("F138", F138))
    board.add_part("F154", PartModel("F154", F154))
