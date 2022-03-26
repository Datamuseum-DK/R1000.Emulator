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

import sys
import os

import transit

from board import Board
from srcfile import SrcFile

ME = os.path.basename(__file__)

def main():
    ''' ... '''
    try:
        open("NetList/" + os.path.basename(__file__))
    except FileNotFoundError:
        sys.stderr.write("Must run %s from root of R1000.Emulator project\n" % ME)
        sys.exit(2)

    os.makedirs("Chassis", exist_ok=True)
    chf = SrcFile("Chassis/plane_tbl.h")
    transit.make_transit_h(chf)
    chf.commit()

    if len(sys.argv) < 3:
        sys.stderr.write("Usage:\n\t%s <branch> <KiCad netlist-file> ...\n" % ME)
        sys.exit(2)

    branch = sys.argv[1]

    for filename in sys.argv[2:]:
        t_new = max(os.stat(filename).st_mtime, os.stat(__file__).st_mtime)
        basename = os.path.basename(filename).split(".", 1)[0]
        speculative_name = basename.capitalize() + "/" + branch
        try:
            t_old = os.stat(speculative_name + "/_timestamp").st_mtime
        except FileNotFoundError:
            t_old = 0
        if t_new > t_old or False:
            print()
            print("Processing", filename)
            board = Board(filename, branch)
            board.produce()
        else:
            print("Skipping", filename, "is older than", speculative_name)

if __name__ == "__main__":
    main()
