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
   Global Clock Generator
   ======================
'''

from part import PartModel, PartFactory


class XClkGen(PartFactory):

    ''' Null component model '''

    def state(self, file):
        file.write("\tunsigned pit;\n")
        file.write("\tunsigned when;\n")

    def sensitive(self):
        for a in range(0):
            yield a

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	unsigned now;
		|
		|	PIN_CLK_DIS<=(0);
		|	now = state->when;
		|	switch (now) {
		|	case 0:
		|		if (++state->pit == 32) {
		|			pit_clock();
		|			state->pit = 0;
		|		}
		|		PIN_BP_2X<=(1); PIN_BP_2Xnot<=(0);
		|		PIN_BP_PHASE<=(0);
		|		state->when = 5;
		|		break;
		|	case 5:
		|		PIN_2XE<=(1); PIN_2XEnot<=(0);
		|		state->when = 10;
		|		break;
		|	case 10:
		|		PIN_2X<=(1); PIN_2Xnot<=(0);
		|		PIN_H1E<=(1); PIN_H2E<=(0);
		|		PIN_Q4<=(1); PIN_Q1<=(0);
		|		state->when = 20;
		|		break;
		|	case 20:
		|		PIN_H1<=(1); PIN_H2<=(0);
		|		state->when = 50;
		|		break;
		|	case 50:
		|		PIN_BP_2X<=(0); PIN_BP_2Xnot<=(1);
		|		state->when = 55;
		|		break;
		|	case 55:
		|		PIN_2XE<=(0); PIN_2XEnot<=(1);
		|		state->when = 60;
		|		break;
		|	case 60:
		|		PIN_2X<=(0); PIN_2Xnot<=(1);
		|		PIN_H1PHD<=(1); PIN_H2PHD<=(0);
		|		PIN_Q1<=(1); PIN_Q2<=(0);
		|		state->when = 100;
		|		break;
		|	case 100:
		|		PIN_BP_2X<=(1); PIN_BP_2Xnot<=(0);
		|		PIN_BP_PHASE<=(1);
		|		state->when = 105;
		|		break;
		|	case 105:
		|		PIN_2XE<=(1); PIN_2XEnot<=(0);
		|		state->when = 110;
		|		break;
		|	case 110:
		|		PIN_2X<=(1); PIN_2Xnot<=(0);
		|		PIN_H1E<=(0); PIN_H2E<=(1);
		|		PIN_Q2<=(1); PIN_Q3<=(0);
		|		state->when = 120;
		|		break;
		|	case 120:
		|		PIN_H1<=(0); PIN_H2<=(1);
		|		state->when = 150;
		|		break;
		|	case 150:
		|		PIN_BP_2X<=(0); PIN_BP_2Xnot<=(1);
		|		state->when = 155;
		|		break;
		|	case 155:
		|		PIN_2XE<=(0); PIN_2XEnot<=(1);
		|		state->when = 160;
		|		break;
		|	case 160:
		|		PIN_2X<=(0); PIN_2Xnot<=(1);
		|		PIN_H1PHD<=(0); PIN_H2PHD<=(1);
		|		PIN_Q3<=(1); PIN_Q4<=(0);
		|		state->when = 200;
		|	}
		|	next_trigger((state->when - now) % 200, SC_NS);
		|	state->when = state->when % 200;
		|
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("XCLKGEN", PartModel("XCLKGEN", XClkGen))

