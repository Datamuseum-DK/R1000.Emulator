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
   Pass: Sort out PU/PD
   ====================
'''

class PassPuPd():

    ''' Pass: Collect all PU/PD nets and unconnected inputs '''

    def __init__(self, cpu):
        self.cpu = cpu
        self.netbusses = {}

        # Tie all the supply networks together in planes.P[DU]
        # ----------------------------------------------------
        for _gnam, net in sorted(self.cpu.nets.items()):
            if not net.is_supply:
                continue
            for node in list(net.nnodes):
                if node.component.is_plane or node.component.is_supply:
                    node.remove()
                    continue
                if node.pin.type.output:
                    print("Bad PUPD Node", node)
            if not net.nnodes:
                continue
            if net.is_pd():
                cpu.plane.psig["PD"].add_net(net)
            elif net.is_pu():
                cpu.plane.psig["PU"].add_net(net)

        # Tie all unconnected inputs to planes.PU
        # ---------------------------------------
        for _gnam, net in sorted(self.cpu.nets.items()):
            if len(net.nnodes) != 1:
                continue
            if not net.nnodes[0].pin.type.output:
                net.name = "PU"
                net.is_supply = True
                cpu.plane.psig["PU"].add_net(net)
