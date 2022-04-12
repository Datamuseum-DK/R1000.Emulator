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
   F652 Transceivers/Registers
   ===========================

   Ref: Fairchild DS009581 March 1988, Revised January 2004
'''


from part import PartModel, PartFactory

class F652(PartFactory):

    ''' F652 Transceivers/Registers '''

    def state(self, file):
        file.fmt('''
		|	uint64_t areg, breg;
		|''')

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        file.fmt('''
		|	uint64_t a = 0;
		|	uint64_t b = 0;
		|	char what[12];
		|
		|	what[0] = '\\0';
		|
		|	if (!PIN_OEA=> && PIN_OEB=> && !PIN_SAB=> && !PIN_SBA=>) {
		|		/* ignore "passive latch" which happens during startup */
		|		TRACE("latch a " << BUS_A_TRACE() << " b " << BUS_B_TRACE());
		|		return;
		|	}
		|
		|	if (PIN_OEA=>) {
		|		BUS_A_Z();
		|		BUS_A_READ(a);
		|		strcat(what, "Ai");
		|	} else if (PIN_SBA=>) {
		|		strcat(what, "Ar");
		|		a = state->breg;
		|	} else {
		|		strcat(what, "Ab");
		|		BUS_B_READ(a);
		|	}
		|
		|	if (!PIN_OEB=>) {
		|		BUS_B_Z();
		|		BUS_B_READ(b);
		|		strcat(what, "Bi");
		|	} else if (PIN_SAB=>) {
		|		strcat(what, "Br");
		|		b = state->areg;
		|	} else {
		|		strcat(what, "Ba");
		|		BUS_A_READ(b);
		|	}
		|
		|	if (PIN_CAB.posedge()) {
		|		state->areg = a;
		|		strcat(what, "A<");
		|	}
		|
		|	if (PIN_CBA.posedge()) {
		|		state->breg = b;
		|		strcat(what, "B<");
		|	}
		|
		|	if (!PIN_OEA=>)
		|		BUS_A_WRITE(a);
		|
		|	if (PIN_OEB=>)
		|		BUS_B_WRITE(b);
		|
		|	TRACE(
		|	    << what
		|	    << " oea " << PIN_OEA?
		|	    << " oeb " << PIN_OEB?
		|	    << " sab " << PIN_SAB?
		|	    << " sba " << PIN_SBA?
		|	    << " cab " << PIN_CAB.posedge()
		|	    << " cba " << PIN_CBA.posedge()
		|	    << " a " << BUS_A_TRACE()
		|	    << " b " << BUS_B_TRACE()
		|	    << " | a "
		|	    << std::hex << a
		|	    << " ra "
		|	    << std::hex << state->areg
		|	    << " b "
		|	    << std::hex << b
		|	    << " rb "
		|	    << std::hex << state->breg
		|	);
		|
		|''')

class ModelF652(PartModel):
    ''' F652 model '''

    def assign(self, comp):
        for node in comp:
            if node.pin.name[:1] in ("A", "B"):
                node.pin.role = "sc_inout_resolved"
        super().assign(comp)

def register(board):
    ''' Register component model '''

    board.add_part("F652", ModelF652("F652", F652))
