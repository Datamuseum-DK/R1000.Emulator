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
   Turn kicad netlist files into SystemC source code
   =================================================
'''

class Node():
    ''' A `node` from the netlist file '''
    def __init__(self, net, sexp):
        self.net = net
        self.sexp = sexp
        self.refname = sexp[0][0].name
        self.pinno = sexp[1][0].name
        self.pinfunction = sexp.find_first("pinfunction")
        if self.pinfunction:
            self.pinfunction = self.pinfunction[0].name
            self.sortkey = (self.pinfunction, 0)
            for i in range(len(self.pinfunction)):
                if self.pinfunction[i].isdigit():
                    try:
                        self.sortkey = (self.pinfunction[:i], int(self.pinfunction[i:]))
                    except ValueError:
                        pass
                    break
        else:
            self.sortkey = (self.pinno, 0)
        self.component = self.net.board.components[self.refname]
        # print("XXX", self, self.pinfunction, self.sortkey)

    def __lt__(self, other):
        return self.sortkey < other.sortkey

    def __repr__(self):
        return "_".join(
            (
                 str(self.net),
                 str(self.component),
                 str(self.pinno),
                 str(self.pinfunction),
            )
        )
