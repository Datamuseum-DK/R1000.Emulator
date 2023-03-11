#!/usr/local/bin/python3
#
# Copyright (c) 2022 Poul-Henning Kamp
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
   Pass: Expand Bus-Pins to normal pins
   ====================================
'''

from node import Node
from pin import Pin

class PassBusPins():

    ''' Pass: Expand bus-pins '''

    def __init__(self, board):
        self.board = board
        for comp in board.components.values():
            buspins = {}
            for node in comp:
                bp = node.pin.buspin()
                if bp is None:
                    continue
                i = buspins.setdefault(bp[0], list())
                i.append((bp[1], node))
                # print("BP", comp, node.pin.rawname, bp)
            for bus, desc in buspins.items():
                if len(desc) != 2:
                    continue
                desc.sort()
                n_lo, node_lo = desc[0]
                assert n_lo == "0"
                n_hi, node_hi = desc[1]
                nn_lo = node_lo.net.name
                nn_hi = node_hi.net.name
                nn_i_lo = ""
                nn_i_hi = ""
                while nn_lo[-1].isdigit():
                    nn_i_lo = nn_lo[-1] + nn_i_lo
                    nn_lo = nn_lo[:-1]
                while nn_hi[-1].isdigit():
                    nn_i_hi = nn_hi[-1] + nn_i_hi
                    nn_hi = nn_hi[:-1]
                print("BUSPINS", comp, nn_lo, nn_i_lo, "…", nn_i_hi, nn_lo)
                assert nn_lo == nn_hi
                for idx, pin_no in enumerate(range(int(n_lo), int(n_hi) + 1)):
                    net = board.nets.get(nn_lo + str(pin_no))
                    assert net
                    new_pin = Pin(nn_lo + str(idx), bus + str(idx), node_lo.pin.role)
                    new_node = Node(net, comp, new_pin)
                node_lo.remove()
                node_hi.remove()
