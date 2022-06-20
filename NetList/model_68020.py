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
   MC68020 CPU
   ===========

'''

from part import PartModel, PartFactory

class MC68020(PartFactory):

    ''' MC68020 CPU'''

    def sensitive(self):
        yield "PIN_CLK.pos()"

    def extra(self, file):
        super().extra(file)
        # The meat of the doit() function lives in a separate file so
        # that fidling with it does not require a rerun of the python code.
        self.scm.cc.include("Ioc/ioc_sc_68k20.hh")

    def state(self, file):
        ''' Extra state variable '''

        file.write("\tstruct ioc_sc_bus_xact *xact;\n")

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        # The meat of the doit() function lives in a separate file so
        # that fidling with it does not require a rerun of the python code.

        self.scm.cc.include("Ioc/ioc_sc_68k20.cc")

def register(board):
    ''' Register component model '''

    board.add_part("68020", PartModel("68020", MC68020))
