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
import glob

from board import Board
from srcfile import SrcFile, Makefile
from scmod import SC_Mod

import models

import planes

from pass_pupd import PassPuPd
from pass_assign_part import PassAssignPart
from pass_net_config import PassNetConfig
from pass_part_config import PassPartConfig
from pass_bus_pins import PassBusPins

ME = os.path.basename(__file__)

class R1000Cpu():

    ''' A R1000 CPU '''

    def __init__(self, workdir, branch, netlists):
        ''' ... '''
        self.workdir = workdir
        self.branch = branch
        self.netlists = netlists
        self.part_catalog = {}
        self.boards = []
        self.chassis_makefile = None
        self.nets = {}
        self.plane = None
        self.z_codes = []

        self.cdir = os.path.join(workdir, "Chassis")
        self.tstamp = os.path.join(self.cdir, "_timestamp")
        self.planes_hh = os.path.join(self.cdir, "planes.hh")

        os.makedirs(self.cdir, exist_ok=True)

        if self.already_current():
            print("Already up to date")
            return

        models.register(self)

        self.do_build()
        open(self.tstamp, "w").write("\n")
        self.report_bom()

    def add_part(self, name, part):
        ''' Add a part to our catalog, if not already occupied '''
        if name not in self.part_catalog:
            self.part_catalog[name] = part

    def add_z_code(self, comp, zcode):
        self.z_codes.append((comp, zcode))

    def sc_mod(self, basename):
        ''' ... '''
        return SC_Mod(os.path.join(self.cdir, basename), self.chassis_makefile)

    def already_current(self):
        ''' Check if generated files are newer than netlists '''
        try:
            t_old = os.stat(self.tstamp).st_mtime
        except FileNotFoundError:
            return False

        try:
            if os.stat("Makefile.local").st_mtime > t_old:
                print("Makefile.local", "triggers build")
                return False
        except FileNotFoundError:
            pass

        if os.stat(__file__).st_mtime > t_old:
            print(__file__, "triggers build")
            return False

        for filename in self.netlists:
            if os.stat(filename).st_mtime > t_old:
                print(filename, "triggers build")
                return False

        for fn in glob.glob("NetList/*.py"):
            if os.stat(fn).st_mtime > t_old:
                print(fn, "triggers build")
                return False

        return True

    def do_build(self):
        ''' ... '''

        self.chassis_makefile = Makefile(os.path.join(self.cdir, "Makefile.inc"))

        for filename in self.netlists:
            print("Processing", filename)
            self.boards.append(Board(self, filename))

        # Establish canonical order
        self.boards.sort()

        self.plane = planes.Planes(self)

        for board in self.boards:
            PassBusPins(board)

        PassPuPd(self)

        PassAssignPart(self)

        self.plane.build_planes()

        PassNetConfig(self)

        self.plane.produce()

        for board in self.boards:
            PassPartConfig(board)

        for board in self.boards:
            board.produce()

        self.chassis_makefile.commit()

        self.emit_z_codes()

    def emit_z_codes(self):

        z_codes = SrcFile(self.cdir + "/z_codes.h")

        z_codes.write("\n#define Z_CODES \\\n")

        for comp, z_code in sorted(self.z_codes):
            z_codes.write('\tZ_CODE(' + z_code + ', "' + str(comp) + '") \\\n')

        z_codes.write('\n')

        z_codes.commit()

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

    if len(sys.argv) < 4:
        sys.stderr.write("Usage:\n\t%s <workdir> <branch> <KiCad netlist-file> …\n" % ME)
        sys.exit(2)

    workdir = sys.argv[1]
    branch = sys.argv[2]

    R1000Cpu(workdir, branch, sys.argv[3:])

if __name__ == "__main__":
    main()
