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
Chip models
'''

from component import Component, VirtualComponent

from model_sram import ModelSRAM
from model_f24x import ModelF24x
from model_f280 import ModelF280
from model_f374 import ModelXREG
from model_f521 import ModelF521
from model_paxxx import ModelPAxxx

def Model(board, sexp):
    ''' Find the right (sub)class or this component '''
    part = sexp.find_first("libsource.part")[0].name
    cls = {
        '2167': ModelSRAM,
        'F240': ModelF24x,
        'F244': ModelF24x,
        'F280': ModelF280,
        'F374': ModelXREG,
        'F521': ModelF521,
        'GB': VirtualComponent,
        'GF': VirtualComponent,
        'PAxxx': ModelPAxxx,
        'Pull_Up': VirtualComponent,
        'Pull_Down': VirtualComponent,
    }.get(part, Component)
    return cls(board, sexp)
