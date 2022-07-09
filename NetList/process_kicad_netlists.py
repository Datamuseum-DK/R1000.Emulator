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

from board import Board
from srcfile import SrcFile
from scmod import SC_Mod

import models

from pass_assign_part import PassAssignPart
from pass_planes import PassPlanes
from pass_net_config import PassNetConfig
from pass_part_config import PassPartConfig

ME = os.path.basename(__file__)

class R1000Cpu():

    ''' A R1000 CPU '''

    def __init__(self, branch, netlists):
        ''' ... '''
        self.branch = branch
        self.netlists = netlists
        self.part_catalog = {}
        self.boards = []
        self.chassis_makefile = None

        os.makedirs("Chassis", exist_ok=True)
        os.makedirs(os.path.join("Chassis", branch), exist_ok=True)

        if self.already_current():
            print("Already up to date")
            return

        models.register(self)

        self.do_build()
        open(os.path.join("Chassis", self.branch, "_timestamp"), "w").write("\n")
        self.report_bom()

    def add_part(self, name, part):
        ''' Add a part to our catalog, if not already occupied '''
        if name not in self.part_catalog:
            self.part_catalog[name] = part

    def sc_mod(self, basename):
        ''' ... '''
        return SC_Mod(os.path.join("Chassis", self.branch, basename), self.chassis_makefile)

    def already_current(self):
        ''' Check if generated files are newer than netlists '''
        try:
            t_old = os.stat(os.path.join("Chassis", self.branch, "_timestamp")).st_mtime
        except FileNotFoundError:
            return False

        if os.stat(__file__).st_mtime > t_old:
            print(__file__, "triggers build")
            return False

        for filename in self.netlists:
            if os.stat(filename).st_mtime > t_old:
                print(filename, "triggers build")
                return False

        return True

    def do_build(self):
        ''' ... '''

        self.chassis_makefile = SrcFile(os.path.join("Chassis", self.branch, "Makefile.inc"))

        for filename in self.netlists:
            print("Processing", filename)
            self.boards.append(Board(self, filename))

        self.boards.sort()

        PassAssignPart(self)
        PassPlanes(self)

        for board in self.boards:
            PassNetConfig(board)

        for board in self.boards:
            PassPartConfig(board)

        for board in self.boards:
            board.produce()

        self.chassis_makefile.commit()

    def report_bom(self):
        ''' Report component usage '''
        with open("_bom.txt", "w") as file:
            file.write("-" * 40 + '\n')
            parts = set(self.part_catalog.values())
            file.write("%6d         Total parts\n" % len(parts))
            file.write("%6d         Total components\n" % sum(len(x.uses) for x in parts))
            file.write("-" * 40 + '\n')
            file.write("\n")
            file.write("  Uses  Blame  Part\n")
            file.write("-" * 40 + '\n')
            for part in sorted(parts, key=lambda x: (-len(x.blame), len(x.uses))):
                if not part.uses and not part.blame:
                    continue
                file.write("%6d " % len(part.uses))
                if part.blame:
                    file.write(" %6d  " % len(part.blame))
                else:
                    file.write("      -  ")
                file.write(" " + part.name + "\n")
                for i, comp in enumerate(
                    sorted(
                        part.uses,
                        key=lambda x: (x.board.name, x.name)
                    )
                ):
                    if i == 5:
                        file.write("\t\t    …\n")
                        break
                    file.write("\t\t    " + comp.board.name + "::" + comp.name + "\n")
            file.write("-" * 40 + '\n')

def main():
    ''' ... '''

    try:
        open("NetList/" + os.path.basename(__file__))
    except FileNotFoundError:
        sys.stderr.write("Must run %s from root of R1000.Emulator project\n" % ME)
        sys.exit(2)

    if len(sys.argv) < 3:
        sys.stderr.write("Usage:\n\t%s <branch> <KiCad netlist-file> …\n" % ME)
        sys.exit(2)

    branch = sys.argv[1]

    R1000Cpu(branch, sys.argv[2:])

if __name__ == "__main__":
    main()
