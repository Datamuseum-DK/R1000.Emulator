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
        file.write("\n# " + self.basename + "\n")
        obj = self.basename + ".o"
        file.write("OBJS += " + obj + "\n")
        file.write(obj + ":")
        for incl in sorted(self.cc):
            file.write(" \\\n    " + incl)
        file.write("\n")
        file.write("\t${SC_CC} -o " + obj + " " + self.cc.filename + "\n")

    def std_hh(self, module, pin_iterator):
        self.hh.write("#ifndef R1000_%s\n" % module)
        self.hh.write("#define R1000_%s\n" % module)
        self.hh.write("\n")
        self.hh.write("struct scm_%s_state;\n" % module.lower())
        self.hh.write("\n")
        self.hh.write("SC_MODULE(SCM_%s)\n" % module)
        self.hh.write("{\n")
        for i in pin_iterator:
            self.hh.write("\t" + i + "\n")
        self.hh.write("\n")
        self.hh.write("\tSC_HAS_PROCESS(SCM_%s);\n" % module)
        self.hh.write("\n")
        self.hh.write("\tSCM_%s(sc_module_name nm, const char *arg);\n" % module)
        self.hh.write("\n")
        self.hh.write("\tprivate:")
        self.hh.write("\n")
        self.hh.write("\tstruct scm_%s_state *state;\n" % module.lower())
        self.hh.write("\tvoid doit(void);\n")
        self.hh.write("};\n")
        self.hh.write("\n")
        self.hh.write("#endif /* R1000_%s */\n" % module)

    def std_cc(self, module, state, init, sensitive, doit):
        self.cc.write("#include <systemc.h>\n")
        self.cc.include("Chassis/r1000sc.h")
        self.cc.include("Infra/context.h")
        self.cc.include(self.hh)
        self.cc.write("\n")
        self.cc.write("struct scm_%s_state {\n" % module.lower())
        self.cc.write("\tstruct ctx ctx;\n")
        if state:
            state(self.cc)
        self.cc.write("};\n")
        self.cc.write("\n")
        self.cc.write("SCM_%s :: SCM_%s(sc_module_name nm, const char *arg) : sc_module(nm)\n" % (module, module))
        self.cc.write("{\n")
        self.cc.write("\tstate = (struct scm_%s_state *)\n" % module.lower())
        self.cc.write('\tCTX_Get("%s", this->name(), sizeof *state);\n' % module.lower())
        self.cc.write("\tshould_i_trace(this->name(), &state->ctx.do_trace);\n")
        if init:
            self.cc.write("\n")
            init(self.cc)
        self.cc.write("\n")
        self.cc.write("\tSC_METHOD(doit);\n")
        self.cc.write("\tsensitive\n\t    << ")
        self.cc.write("\n\t    << ".join(sensitive()) + ";\n")
        self.cc.write("}\n")
        self.cc.write("\n")
        self.cc.write("void\n")
        self.cc.write("SCM_%s :: doit(void)\n" % module)
        self.cc.write("{\n")
        self.cc.write("\tstate->ctx.activations++;\n")
        self.cc.write("\n")
        doit(self.cc)
        self.cc.write("}\n")
