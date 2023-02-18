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

import os

from srcfile import SrcFile

class SC_Mod():
    ''' A SystemC source module '''
    def __init__(self, filename, makefile=None):
        self.filename = filename
        self.makefile = makefile
        self.basename = os.path.split(filename)[-1]
        self.cc = SrcFile(filename + ".cc")
        self.hh = SrcFile(filename + ".hh")
        self.pub = SrcFile(filename + "_pub.hh")
        self.subst("«mmm»", self.basename.upper())
        self.subst("«lll»", self.basename.lower())

    def commit(self):
        ''' ... '''
        self.cc.commit()
        self.hh.commit()
        self.pub.commit()
        if self.makefile:
            self.makefile_entry(self.makefile)

    def subst(self, find, replace):
        ''' Add substituation patterns '''
        for file in (self.cc, self.hh, self.pub):
            file.subst(find, replace)

    def makefile_entry(self, file):
        ''' ... '''
        bname = self.filename.split("/")[-1]
        hdr = "# " + bname + "\n"
        txt = ""
        obj = "${OBJDIR}/" + bname + ".o"
        txt += "OBJS += " + obj + "\n"
        txt += obj + ":"
        for incl in sorted(self.cc):
            txt += " \\\n    " + incl
        txt += "\n"
        txt += "\t${SC_CC} -o " + obj + " " + self.cc.filename + "\n"
        file.add_stanza(hdr, txt)

    def std_hh(self, pin_iterator):
        ''' Produce a stanard .hh file '''
        self.hh.fmt('''
		|#ifndef R1000_«mmm»
		|#define R1000_«mmm»
		|
		|struct scm_«lll»_state;
		|
		|SC_MODULE(SCM_«mmm»)
		|{
		|''')

        for i in pin_iterator:
            self.hh.write("\t" + i + "\n")

        self.hh.fmt('''
		|
		|	SC_HAS_PROCESS(SCM_«mmm»);
		|
		|	SCM_«mmm»(sc_module_name nm, const char *arg);
		|
		|	private:
		|	struct scm_«lll»_state *state;
		|	void doit(void);
		|};
		|
		|#endif /* R1000_«mmm» */
		|''')

    def std_cc(self, extra=None, state=None, init=None, sensitive=None, doit=None):
        ''' Produce a stanard .cc file '''
        self.cc.write("#include <systemc.h>\n")
        self.cc.include("Chassis/r1000sc.h")
        self.cc.include("Infra/context.h")
        self.cc.write("\n")
        self.cc.include(self.hh)

        if extra:
            self.cc.write("\n")
            extra(self.cc)

        self.cc.fmt('''
		|
		|struct scm_«lll»_state {
		|	struct ctx ctx;
		|''')

        if state:
            state(self.cc)

        self.cc.fmt('''
		|};
		|
		|SCM_«mmm» ::
		|    SCM_«mmm»(sc_module_name nm, const char *arg)
		|	: sc_module(nm)
		|{
		|	state = (struct scm_«lll»_state *)
		|	    CTX_Get("«lll»", this->name(), sizeof *state);
		|	should_i_trace(this->name(), &state->ctx.do_trace);
		|''')

        if init:
            self.cc.write("\n")
            init(self.cc)

        self.cc.fmt('''
		|
		|	SC_METHOD(doit);
		|''')

        i = list(sensitive())
        if i:
            self.cc.write("\tsensitive\n\t    << ")
            self.cc.write("\n\t    << ".join(sensitive()) + ";\n")

        self.cc.fmt('''
		|}
		|
		|void
		|SCM_«mmm» :: doit(void)
		|{
		|	state->ctx.activations++;
		|
		|''')

        doit(self.cc)

        self.cc.write("}\n")
