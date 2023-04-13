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
   A "board" is the contents of a single KiCad Netlist file (=project)
   ===================================================================
'''

import os

from srcfile import Makefile
from scmod import SystemCModule

class Board(SystemCModule):

    ''' Ingest one KiCad netlist file '''

    def __init__(self, cpu, name):
        name = name.lower()
        self.cpu = cpu

        self.dstdir = os.path.join(cpu.workdir, name.capitalize())
        os.makedirs(self.dstdir, exist_ok=True)

        self.makefile = Makefile(self.dstdir + "/Makefile.inc")

        super().__init__(self.sc_path(name), self.makefile)
        self.sc_fixup(self)
        self.add_ctor_arg("struct planes", "planes", is_ptr=True)

        # self.add_ctor_arg("struct planes", "planes", is_ptr = True)

        self.scm_globals = self.sc_mod(self.scm_lname + "_globals")
        self.scm_globals.scm_cname_pfx = self.scm_lname + "_globals->"
        self.add_child(self.scm_globals)

    def sc_path(self, basename):
        ''' Source path of a SCM on this board '''
        return os.path.join(self.dstdir + "/" + basename)

    def add_child(self, scm):
        self.sc_fixup(scm)
        if scm != self.scm_globals:
            scm.add_ctor_arg("struct planes", "planes", is_ptr=True)
            scm.add_ctor_arg("struct «bbb»_globals", "«bbb»_globals", is_ptr=True)
            scm.include(self.scm_globals.sf_hh)
            scm.include(self.cpu.plane.sf_hh)
        super().add_child(scm)


    def sc_fixup(self, scm):
        ''' Add board substitutions '''
        scm.add_subst("«bbb»", self.scm_lname)
        scm.add_subst("«BBB»", self.scm_uname)

    def sc_mod(self, basename):
        ''' Make a SCM which lives on this board '''
        scm = SystemCModule(
            self.sc_path(basename),
            self.makefile
        )
        self.sc_fixup(scm)
        return scm

    def produce(self):
        ''' ... '''
        os.makedirs(self.dstdir, exist_ok=True)

        self.emit_pub_hh()
        self.emit_hh()
        self.emit_cc()
        self.commit()

        for net in self.scm_globals.iter_nets():
            for sig in net.sc_signals():
                self.scm_globals.add_signal(sig)

        self.scm_globals.emit_pub_hh()
        self.scm_globals.emit_hh()
        self.scm_globals.emit_cc()
        self.scm_globals.commit()

        for sheet in self.scm_children.values():
            sheet.produce()

        self.makefile.commit()
