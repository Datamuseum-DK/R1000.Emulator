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
   Fetch the R1000 firmware files from the Datamuseum.dk BitStore
   --------------------------------------------------------------
'''

import sys
import os
import http.client

FWPATH = '_Firmware'

conn = http.client.HTTPSConnection('datamuseum.dk')

def getone(nbr):
    ''' Fetch one artifact from the Bitstore '''

    dst = FWPATH + "/%d.bin" % nbr
    try:
        inode = os.stat(dst)
        if inode.st_nlink == 2:
            return
    except FileNotFoundError:
        pass

    conn.request("GET", "/bits/%d" % nbr)
    resp = conn.getresponse()
    body = resp.read()
    open(dst, "wb").write(body)

    for hdr in resp.getheaders():
        if hdr[0] == 'Content-Disposition':
            flds = hdr[1].split('"')
            assert flds[0] == 'attachment; filename='
            assert flds[2] == ''
            assert '..' not in flds[1]
            assert '/' not in flds[1]
            os.link(dst, FWPATH + "/" + flds[1])
            print("Got", nbr, flds[1])
            break

def firmwarelist():
    ''' See: https://datamuseum.dk/wiki/Bits:Keyword/RATIONAL_1000/SW '''
    yield from range(30000502, 30000504)
    if sys.argv[1:] == ["all"]:
        yield from range(30002508, 30002518)
        yield from range(30002520, 30002616)
        yield from range(30002631, 30002641)
        yield from range(30003041, 30003042)
        yield from range(30003101, 30003107)
        yield from range(30002889, 30002896)	# MEM32 GAL
        yield from range(30002896, 30002903)	# MEM32 DFSM

try:
    os.mkdir(FWPATH)
except FileExistsError:
    pass

for i in firmwarelist():
    getone(i)
