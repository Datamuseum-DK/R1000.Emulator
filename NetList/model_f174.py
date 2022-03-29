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
   F174 Hex D-Type Flip-Flop with Master Reset
   ===================================

   Ref: Fairchild DS009489 April 1988 Revised September 2000
'''


from part import PartModel, PartFactory

class F174(PartFactory):

    ''' F174 9-Bit Parity Generator Checker '''

    def state(self, file):
        file.fmt('''
		|	bool dreg[6];
		|	int job;
		|''')

    def init(self, file):
        file.fmt('''
		|	state->job = 1;
		|''')

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|
		|	const char *what = "";
		|	bool nxt[6];
		|
		|	TRACE(
		|	    << what
		|	    << " clr_" << PIN_CLR?
		|	    << " clk " << PIN_CLK?
		|	    << " d " << PIN_D0?
		|	    << PIN_D1?
		|	    << PIN_D2?
		|	    << PIN_D3?
		|	    << PIN_D4?
		|	    << PIN_D5?
		|	    << " | "
		|	    << state->dreg[0]
		|	    << state->dreg[1]
		|	    << state->dreg[2]
		|	    << state->dreg[3]
		|	    << state->dreg[4]
		|	    << state->dreg[5]
		|	);
		|	if (state->job) {
		|		PIN_Q0<=(state->dreg[0]);
		|		PIN_Q1<=(state->dreg[1]);
		|		PIN_Q2<=(state->dreg[2]);
		|		PIN_Q3<=(state->dreg[3]);
		|		PIN_Q4<=(state->dreg[4]);
		|		PIN_Q5<=(state->dreg[5]);
		|		state->job = 0;
		|	}
		|	if (!PIN_CLR=>) {
		|		nxt[0] = false;
		|		nxt[1] = false;
		|		nxt[2] = false;
		|		nxt[3] = false;
		|		nxt[4] = false;
		|		nxt[5] = false;
		|		what = " CLR ";
		|	} else if (PIN_CLK.posedge()) {
		|		nxt[0] = PIN_D0=>;
		|		nxt[1] = PIN_D1=>;
		|		nxt[2] = PIN_D2=>;
		|		nxt[3] = PIN_D3=>;
		|		nxt[4] = PIN_D4=>;
		|		nxt[5] = PIN_D5=>;
		|		what = " CLK ";
		|	} else {
		|		nxt[0] = state->dreg[0];
		|		nxt[1] = state->dreg[1];
		|		nxt[2] = state->dreg[2];
		|		nxt[3] = state->dreg[3];
		|		nxt[4] = state->dreg[4];
		|		nxt[5] = state->dreg[5];
		|		what = " ??? ";
		|	}
		|	if (
		|	    nxt[0] != state->dreg[0] ||
		|	    nxt[1] != state->dreg[1] ||
		|	    nxt[2] != state->dreg[2] ||
		|	    nxt[3] != state->dreg[3] ||
		|	    nxt[4] != state->dreg[4] ||
		|	    nxt[5] != state->dreg[5]
		|	) {
		|		state->job = 1;
		|		state->dreg[0] = nxt[0];
		|		state->dreg[1] = nxt[1];
		|		state->dreg[2] = nxt[2];
		|		state->dreg[3] = nxt[3];
		|		state->dreg[4] = nxt[4];
		|		state->dreg[5] = nxt[5];
		|		next_trigger(5, SC_NS);
		|	}
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("F174", PartModel("F174", F174))
