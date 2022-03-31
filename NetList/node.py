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

import pin

class Node():

    ''' A `node` connects a `net` with a `component`'s `pin` '''

    def __init__(self, net, component, pinspec):
        self.net = net
        self.component = component
        self.refname = component.name
        self.pin = pinspec
        self.sortkey = (self.component, self.pin)
        self.insert()

    def __lt__(self, other):
        return self.sortkey < other.sortkey

    def insert(self):
        ''' ... '''
        self.component.add_node(self)
        self.net.add_node(self)

    def remove(self):
        ''' ... '''
        self.component.del_node(self)
        self.net.del_node(self)

    def __repr__(self):
        return "_".join(
            (
                 str(self.net),
                 str(self.component),
                 str(self.pin),
            )
        )

class NodeSexp(Node):

    ''' Create `node` from netlist-sexp '''

    def __init__(self, net, sexp):
        pinname = sexp.find_first("pinfunction")
        if pinname:
            pinname = pinname[0].name
        else:
            pinname = "W"
        super().__init__(
            net = net,
            component = net.board.get_component(sexp[0][0].name),
            pinspec = pin.Pin(
                pinident=sexp[1][0].name,
                pinname=pinname,
                pinrole=sexp.find_first("pintype")[0].name,
            )
        )
