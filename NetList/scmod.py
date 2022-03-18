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

from srcfile import SrcFile

class SC_Mod():
    ''' A SystemC source module '''
    def __init__(self, basename):
        self.basename = basename
        self.cc = SrcFile(basename + ".cc")
        self.hh = SrcFile(basename + ".hh")
        self.pub = SrcFile(basename + "_pub.hh")

    def commit(self):
        ''' ... '''
        self.cc.commit()
        self.hh.commit()
        self.pub.commit()

    def makefile(self, file):
        ''' ... '''
        file.write("\n#" + self.basename + "\n")
        obj = self.basename + ".o"
        file.write("OBJS += " + obj + "\n")
        file.write(obj + ":")
        for incl in sorted(self.cc):
            file.write(" \\\n    " + incl)
        file.write("\n")
        file.write("\t${SC_CC} -o " + obj + " " + self.cc.filename + "\n")
