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
   AM93S48 12-Bit Parity Generator Checker
   =======================================

'''


from part import PartModel, PartFactory

class AM93S48(PartFactory):

    ''' AM93S48 12-Bit Parity Generator Checker '''

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|
		|	bool s = PIN_I0=>
		|	    ^ PIN_I1=>
		|	    ^ PIN_I2=>
		|	    ^ PIN_I3=>
		|	    ^ PIN_I4=>
		|	    ^ PIN_I5=>
		|	    ^ PIN_I6=>
		|	    ^ PIN_I7=>
		|	    ^ PIN_I8=>
		|	    ^ PIN_I9=>
		|	    ^ PIN_I10=>
		|	    ^ PIN_I11=>;
		|
		|	TRACE(
		|	    << PIN_I0?
		|	    << PIN_I1?
		|	    << PIN_I2?
		|	    << PIN_I3?
		|	    << PIN_I4?
		|	    << PIN_I5?
		|	    << PIN_I6?
		|	    << PIN_I7?
		|	    << PIN_I8?
		|	    << PIN_I9?
		|	    << PIN_I10?
		|	    << PIN_I11?
		|	    << " odd "
		|	    << s
		|	);
		|
		|	PIN_PEV<=(!s);
		|	PIN_POD<=(s);
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("93S48", PartModel("93S48", AM93S48))