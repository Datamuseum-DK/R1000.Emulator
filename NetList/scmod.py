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
   A SystemC Module
   ================
'''

import os

from srcfile import SrcFile

class SystemCModule():
    ''' A SystemC source module '''
    def __init__(self, filename, makefile=None):
        self.filename = filename
        self.makefile = makefile
        self.basename = os.path.split(filename)[-1]
        self.sf_cc = SrcFile(filename + ".cc")
        self.sf_hh = SrcFile(filename + ".hh")
        self.sf_pub = SrcFile(filename + "_pub.hh")
        self.add_subst("«mmm»", self.basename.upper())
        self.add_subst("«lll»", self.basename.lower())

    def commit(self):
        ''' ... '''
        self.sf_cc.commit()
        self.sf_hh.commit()
        self.sf_pub.commit()
        if self.makefile:
            self.makefile_entry(self.makefile)

    def add_subst(self, find, replace):
        ''' Add substituation patterns '''
        for file in (self.sf_cc, self.sf_hh, self.sf_pub):
            file.add_subst(find, replace)

    def makefile_entry(self, makefile):
        ''' ... '''
        bname = self.filename.split("/")[-1]
        hdr = "# " + bname + "\n"
        txt = ""
        obj = "${OBJDIR}/" + bname + ".o"
        txt += "OBJS += " + obj + "\n"
        txt += obj + ":"
        for incl in sorted(self.sf_cc):
            txt += " \\\n    " + incl
        txt += "\n"
        txt += "\t${SC_CC} -o " + obj + " " + self.sf_cc.filename + "\n"
        makefile.add_stanza(hdr, txt)

    def std_hh(self, pin_iterator):
        ''' Produce a stanard .sf_hh file '''
        self.sf_hh.fmt('''
		|#ifndef R1000_«mmm»
		|#define R1000_«mmm»
		|
		|struct scm_«lll»_state;
		|
		|SC_MODULE(SCM_«mmm»)
		|{
		|''')

        for i in pin_iterator:
            self.sf_hh.write("\t" + i + "\n")

        self.sf_hh.fmt('''
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
        ''' Produce a standard .sf_cc file '''
        self.sf_cc.write("#include <systemc.h>\n")
        self.sf_cc.include("Chassis/r1000sc.h")
        self.sf_cc.include("Infra/context.h")
        self.sf_cc.write("\n")
        self.sf_cc.include(self.sf_hh)

        if extra:
            self.sf_cc.write("\n")
            extra(self.sf_cc)

        self.sf_cc.fmt('''
		|
		|struct scm_«lll»_state {
		|	struct ctx ctx;
		|''')

        if state:
            state(self.sf_cc)

        self.sf_cc.fmt('''
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
            self.sf_cc.write("\n")
            init(self.sf_cc)

        self.sf_cc.fmt('''
		|
		|	SC_METHOD(doit);
		|''')

        i = list(sensitive())
        if i:
            self.sf_cc.write("\tsensitive\n\t    << ")
            self.sf_cc.write("\n\t    << ".join(sensitive()) + ";\n")

        self.sf_cc.fmt('''
		|}
		|
		|void
		|SCM_«mmm» :: doit(void)
		|{
		|	state->ctx.activations++;
		|
		|''')

        doit(self.sf_cc)

        self.sf_cc.write("}\n")
