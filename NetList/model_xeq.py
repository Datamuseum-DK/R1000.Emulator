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
   F521 8-Bit Identity Comparator (and multiples)
   ==============================================

   Ref: Fairchild DS009545 April 1988 Revised October 2000
'''


from part import PartModel, PartFactory

class XEQ(PartFactory):

    ''' F521 8-Bit Identity Comparator (and multiples) '''

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	if (PIN_E=>) {
		|		PIN_AeqB<=(true);
		|	} else {
		|		uint64_t a, b;
		|		BUS_A_READ(a);
		|		BUS_B_READ(b);
		|		PIN_AeqB<=((a != b));
		|	}
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("F521", PartModel("F521", XEQ))
    board.add_part("XEQ16", PartModel("XEQ16", XEQ))
    board.add_part("XEQ20", PartModel("XEQ20", XEQ))
    board.add_part("XEQ32", PartModel("XEQ32", XEQ))
    board.add_part("XEQ40", PartModel("XEQ40", XEQ))