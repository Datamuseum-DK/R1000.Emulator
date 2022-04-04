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
   F169 (Dual) D-Type Positive Edge-Triggered Flip-Flop
   ===================================================

   Ref: Fairchild DS009469 April 1988 Revised September 2000
'''


from part import PartModel, PartFactory

class F169(PartFactory):

    ''' F169 (Dual) D-Type Positive Edge-Triggered Flip-Flop '''

    def state(self, file):
        ''' Extra state variable '''

        file.write("\tunsigned count;\n")

    def sensitive(self):
        yield "PIN_CLK.pos()"
        if not self.comp.nodes["ENP"].net.is_const():
            yield "PIN_ENP"
        if not self.comp.nodes["ENT"].net.is_const():
            yield "PIN_ENT"

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	bool carry;
		|	const char *what = NULL;
		|
		|	if (PIN_CLK.posedge()) {
		|		if (!PIN_LD=>) {
		|			what = " ld ";
		|			state->count = 0;
		|			if (PIN_D3=>) state->count |= 1;
		|			if (PIN_D2=>) state->count |= 2;
		|			if (PIN_D1=>) state->count |= 4;
		|			if (PIN_D0=>) state->count |= 8;
		|		} else if (!PIN_ENP=> && !PIN_ENT=>) {
		|			if (PIN_UP=>) {
		|				what = " up ";
		|				state->count = (state->count + 0x1) & 0xf;
		|			} else {
		|				what = " dn ";
		|				state->count = (state->count + 0xf) & 0xf;
		|			}
		|		} else {
		|''')
        i = []
        if not self.comp.nodes["LD"].net.is_const():
            i.append("PIN_LD.negedge_event()")
        if not self.comp.nodes["ENP"].net.is_const():
            i.append("PIN_ENP.default_event()")
        if not self.comp.nodes["ENT"].net.is_const():
            i.append("PIN_ENT.default_event()")

        file.write("\t\t\tnext_trigger(%s);\n" % (" | ".join(i)))

        file.fmt('''
		|		}
		|	}
		|	if (PIN_ENT=>)
		|		carry = true;
		|	else if (PIN_UP=> && state->count == 0xf)
		|		carry = false;
		|	else if (!PIN_UP=> && state->count == 0x0)
		|		carry = false;
		|	else
		|		carry = true;
		|	if ((state->ctx.do_trace & 2) && what == NULL)
		|		what = " - ";
		|	if (what != NULL) {
		|		TRACE(
		|		    << what
		|		    << " up " << PIN_UP?
		|		    << " clk " << PIN_CLK?
		|		    << " d " << PIN_D0? << PIN_D1? << PIN_D2? << PIN_D3?
		|		    << " enp " << PIN_ENP?
		|		    << " load " << PIN_LD?
		|		    << " ent " << PIN_ENT?
		|		    << "|"
		|		    << std::hex << state->count
		|		    << "+"
		|		    << carry
		|		);
		|	}
		|	PIN_Q3<=(state->count & 1);
		|	PIN_Q2<=(state->count & 2);
		|	PIN_Q1<=(state->count & 4);
		|	PIN_Q0<=(state->count & 8);
		|	PIN_CO<=(carry);
		|''')

def register(board):
    ''' Register component model '''

    board.add_part("F169", PartModel("F169", F169))
