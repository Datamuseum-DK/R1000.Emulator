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
   Convert F240 and F244 chips to XBUF8 and XBUF4(s)
   =================================================
'''

import copy

from component import Component
import model

import libpart
import sexp

LIBPART_XBUF4 = '''
    (libpart (lib "r1000") (part "XBUF4")
      (fields
        (field (name "Reference") "U")
        (field (name "Value") "XBUF4")
        (field (name "Location") "___")
        (field (name "Name") "______"))
      (pins
        (pin (num "1") (name "INV") (type "input"))
        (pin (num "2") (name "OE") (type "input"))
        (pin (num "3") (name "I0") (type "input"))
        (pin (num "4") (name "Y0") (type "tri_state"))
        (pin (num "5") (name "I1") (type "input"))
        (pin (num "6") (name "Y1") (type "tri_state"))
        (pin (num "7") (name "I2") (type "input"))
        (pin (num "8") (name "Y2") (type "tri_state"))
        (pin (num "9") (name "I3") (type "input"))
        (pin (num "10") (name "Y3") (type "tri_state"))))
'''

LIBPART_XBUF8 = '''
    (libpart (lib "r1000") (part "XBUF8")
      (fields
        (field (name "Reference") "U")
        (field (name "Value") "XBUF8")
        (field (name "Location") "___")
        (field (name "Name") "______"))
      (pins
        (pin (num "1") (name "INV") (type "input"))
        (pin (num "2") (name "OE") (type "input"))
        (pin (num "3") (name "I0") (type "input"))
        (pin (num "4") (name "Y0") (type "tri_state"))
        (pin (num "5") (name "I1") (type "input"))
        (pin (num "6") (name "Y1") (type "tri_state"))
        (pin (num "7") (name "I2") (type "input"))
        (pin (num "8") (name "Y2") (type "tri_state"))
        (pin (num "9") (name "I3") (type "input"))
        (pin (num "10") (name "Y3") (type "tri_state"))
        (pin (num "11") (name "I4") (type "input"))
        (pin (num "12") (name "Y4") (type "tri_state"))
        (pin (num "13") (name "I5") (type "input"))
        (pin (num "14") (name "Y5") (type "tri_state"))
        (pin (num "15") (name "I6") (type "input"))
        (pin (num "16") (name "Y6") (type "tri_state"))
        (pin (num "17") (name "I7") (type "input"))
        (pin (num "18") (name "Y7") (type "tri_state"))))
'''

class ModelF24x(Component):
    ''' F240/F244 '''

    def configure(self):
        self.fallback = False
        same = self.nodes["OE0"].net == self.nodes["OE1"].net
        if not same:

            if "XBUF4" not in self.board.libparts:
                slp = sexp.SExp("bla")
                slp.parse(LIBPART_XBUF4)
                libpart.LibPart(self.board, slp)

            for nbr, child in enumerate(("A", "B")):

                used = set()
                used.add(len(self.nodes["OE%d" % nbr].net))
                for i in range(4):
                    j = i + nbr * 4
                    used.add(len(self.nodes["I%d" % j].net))
                    used.add(len(self.nodes["Y%d" % j].net))
                if len(used) == 1 and 1 in used:
                    # all terminals on this half are unconnnected
                    continue

                nsexp = copy.deepcopy(self.sexp)

                ref = nsexp.find_first("ref")
                ref[0].name += child

                libsource = nsexp.find_first("libsource")
                libsource[1][0].name = "XBUF4"
                libsource[2][0].name = self.partname

                for fld in nsexp.find("property"):
                    if fld[0][0].name == 'Name':
                        fld[1][0].name = self.name + child

                ncomp = model.Model(self.board, nsexp)

                ncomp.nodes["OE"] = self.nodes["OE%d" % nbr]
                for i in range(4):
                    j = i + nbr * 4
                    ncomp.nodes["I%d" % i] = self.nodes["I%d" % j]
                    ncomp.nodes["I%d" % i].pinfunction = "I%d" % i
                    ncomp.nodes["Y%d" % i] = self.nodes["Y%d" % j]
                    ncomp.nodes["Y%d" % i].pinfunction = "Y%d" % i
                for i in ncomp.nodes.values():
                    i.component = ncomp

        else:

            if "XBUF8" not in self.board.libparts:
                slp = sexp.SExp("bla")
                slp.parse(LIBPART_XBUF8)
                libpart.LibPart(self.board, slp)

            nsexp = copy.deepcopy(self.sexp)
            libsource = nsexp.find_first("libsource")
            libsource[1][0].name = "XBUF8"
            libsource[2][0].name = self.partname

            ncomp = model.Model(self.board, nsexp)

            ncomp.nodes["OE"] = self.nodes["OE0"]
            for i in range(8):
                ncomp.nodes["I%d" % i] = self.nodes["I%d" % i]
                ncomp.nodes["Y%d" % i] = self.nodes["Y%d" % i]
            for i in ncomp.nodes.values():
                i.component = ncomp

    def include_files(self):
        if not self.name:
            yield None

    def instance(self, file):
        return

    def initialize(self, file):
        return

    def hookup(self, file):
        return
