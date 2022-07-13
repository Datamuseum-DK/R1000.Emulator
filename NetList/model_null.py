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
   Null component models
   ======================
'''

from part import PartModel, PartFactory

class SCM2661(PartFactory):

    def sensitive(self):
        for a in range(0):
            yield a

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)
        file.fmt('''
		|	PIN_BRKDET<=(false);
		|''')

class Null(PartFactory):

    ''' Null component model '''

    def sensitive(self):
        for a in range(0):
            yield a

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

def register(board):
    ''' Register component model '''

    board.add_part("1489", PartModel("1489", Null))
    board.add_part("2661B", PartModel("2661B", SCM2661))
    board.add_part("2681", PartModel("2681", Null))
    board.add_part("28256", PartModel("28256", Null))
    board.add_part("58167", PartModel("58167", Null))
