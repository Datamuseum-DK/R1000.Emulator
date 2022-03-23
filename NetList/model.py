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

#from component import Component, VirtualComponent
#
#import model_f181
#import model_f24x
#import model_f280
#import model_f521
#import model_mux2
#import model_paxxx
#import model_sram
#import model_xbuf
#import model_xlat
#import model_xreg
#
#DISPATCH = {
#    #'2149': model_sram.ModelSRAM,
#    #'2167': model_sram.ModelSRAM,
#    #'F157': model_mux2.ModelMux2,
#    #'F158': model_mux2.ModelMux2,
#    #'F181': model_f181.ModelF181,
#    #'F240': model_f24x.ModelF24x,
#    #'F244': model_f24x.ModelF24x,
#    #'F257': model_mux2.ModelMux2,
#    #'F258': model_mux2.ModelMux2,
#    #'F280': model_f280.ModelF280,
#    #'F373': model_xlat.ModelXLAT,
#    #'F374': model_xreg.ModelXREG,
#    #'F521': model_f521.ModelF521,
#    #'PAxxx': model_paxxx.ModelPAxxx,
#    #'XBUF': model_xbuf.ModelXBUF,
#    #'XREG': model_xreg.ModelXREG,
#    #'XLAT': model_xlat.ModelXLAT,
#    #'XMUX16': model_mux2.ModelMux2,
#}
#
## These are not optional
#VIRTUALS = {
#    'GB': VirtualComponent,
#    'GF': VirtualComponent,
#    'Pull_Up': VirtualComponent,
#    'Pull_Down': VirtualComponent,
#}
#
#def Model(board, sexp):
#    ''' Find the right (sub)class or this component '''
#    part = sexp.find_first("libsource.part")[0].name
#
#    cls = VIRTUALS.get(part)
#    if cls:
#        return cls(board, sexp)
#   
#    cls = DISPATCH.get(part)
#
#    if cls is None and part[0] == 'X' and part[:3].isalpha:
#        while part[-1].isdigit():
#            part = part[:-1]
#        cls = DISPATCH.get(part)
#
#    if cls is None:
#        cls = Component
#    return cls(board, sexp)
