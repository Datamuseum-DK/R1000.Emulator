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
   Pins on components
   ==================
'''

import util

class Pin():

    ''' A `pin` on a `component` '''

    def __init__(self, pinident, pinname, pinrole):
        self.ident = pinident	# Not always numeric!
        self.name = pinname
        self.role = pinrole
        self.bus = None
        self.update()

    def update(self):
        ''' Things (may) have changed '''

        for i, j in (
            (">", "gt"),
            ("<", "lt"),
            ("=", "eq"),
            ("~", "not"),
        ):
            self.name = self.name.replace(i, j)
        if not self.name:
            self.name = "_"
        self.sortkey = util.sortkey(self.name)
        if isinstance(self.sortkey[0], int):
            self.sortkey.insert(0, "_")

    def __repr__(self):
        return "_".join(("Pin", self.ident, self.name, self.role))

    def __lt__(self, other):
        return self.sortkey < other.sortkey

class PinSexp(Pin):

    ''' Create `pin` from netlist-sexp '''

    def __init__(self, sexp):
        super().__init__(
            pinident = sexp[0][0].name,
            pinname = sexp[1][0].name,
            pinrole = sexp[2][0].name,
        )

class Bus():
    ''' A set of pins named `<prefix>[0…N]` '''

    def __init__(self, busname, low):
        self.name = busname
        self.low = low
        self.pins = []

    def __repr__(self):
        return "_".join(("Bus", self.busname, self.low, str(len(self.pins))))

    def add_pin(self, pin):
        self.pins.append(pin)
        pin.bus = self
