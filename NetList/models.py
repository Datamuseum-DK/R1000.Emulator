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
   Register component models
   =========================
'''

import model_1meg
import model_16kx4
import model_2149
import model_2167
import model_25s10
import model_25s558
import model_2kx8
import model_64kx1
import model_8641
import model_93s48
import model_f51
import model_f64
import model_f74
import model_f85
import model_f86
import model_f113
import model_f138
import model_f139
import model_f148
import model_f151
import model_f153
import model_f163
import model_f169
import model_f174
import model_f175
import model_f181
import model_f182
import model_f189
import model_f194
import model_f251
import model_f280
import model_f283
import model_f299
import model_f381
import model_f521
import model_f579
import model_ls125
import model_mux2
import model_nand
import model_nor
import model_novram
import model_paxxx
import model_pbxxx
import model_xbuf
import model_xlat
import model_xreg


def register(where):
    ''' Register all component models '''
    model_1meg.register(where)
    model_16kx4.register(where)
    model_2149.register(where)
    model_2167.register(where)
    model_25s10.register(where)
    model_25s558.register(where)
    model_2kx8.register(where)
    model_64kx1.register(where)
    model_8641.register(where)
    model_93s48.register(where)
    model_f51.register(where)
    model_f64.register(where)
    model_f74.register(where)
    model_f85.register(where)
    model_f86.register(where)
    model_f113.register(where)
    model_f138.register(where)
    model_f139.register(where)
    model_f148.register(where)
    model_f151.register(where)
    model_f153.register(where)
    model_f163.register(where)
    model_f169.register(where)
    model_f174.register(where)
    model_f175.register(where)
    model_f181.register(where)
    model_f182.register(where)
    model_f189.register(where)
    model_f194.register(where)
    model_f251.register(where)
    model_f280.register(where)
    model_f283.register(where)
    model_f299.register(where)
    model_f381.register(where)
    model_f521.register(where)
    model_f579.register(where)
    model_ls125.register(where)
    model_mux2.register(where)
    model_nand.register(where)
    model_nor.register(where)
    model_novram.register(where)
    model_paxxx.register(where)
    model_pbxxx.register(where)
    model_xbuf.register(where)
    model_xlat.register(where)
    model_xreg.register(where)
