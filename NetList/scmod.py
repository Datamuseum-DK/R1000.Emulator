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

class CtorArg():
    ''' An argument for the SCM constructor function '''

    def __init__(self, ctype, cname, is_ref = False, is_ptr=False):
        self.ctype = ctype
        self.cname = cname
        self.is_ref = is_ref
        self.is_ptr = is_ptr

    def protoform(self):
        ''' proto type form '''
        txt = self.ctype + " "
        if self.is_ref:
            txt += "&"
        if self.is_ptr:
            txt += "*"
        return txt + self.cname

class ScSignal():
    ''' A SystemC signal '''
    def __init__(self, name, sctype, defval):
        self.name = name
        self.sctype = sctype
        self.defval = defval

    def __lt__(self, other):
        return self.name < other.name

    def decl(self):
        ''' declaration '''
        txt = self.sctype + "\t"
        while len(txt.expandtabs()) < 24:
            txt += "\t"
        txt += self.name + ";"
        return txt

    def init(self):
        ''' initialization '''
        return self.name + '("' + self.name + '", ' + self.defval + ')'

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
        self.ctor_args = []
        self.children = {}
        self.signals = {}
        self.members = []

        self.sf_cc.write("#include <systemc.h>\n")
        self.sf_cc.include("Chassis/r1000sc.h")
        self.sf_cc.include("Infra/context.h")
        self.sf_cc.write("\n")
        self.sf_cc.include(self.sf_hh)

    def include(self, incl):
        ''' add include file '''
        self.sf_hh.include(incl)

    def add_member(self, cstuff):
        ''' add member items in raw C-form '''
        self.members.append(cstuff)

    def add_child(self, name, scm):
        ''' add a child '''
        assert name not in self.children
        assert isinstance(scm, SystemCModule)
        self.children[name] = scm
        self.sf_hh.include(scm.sf_pub)

    def add_signal(self, *args, **kwargs):
        ''' Add a signal '''
        sig = ScSignal(*args, **kwargs)
        assert sig.name not in self.signals
        self.signals[sig.name] = sig

    def add_ctor_arg(self, *args, **kwargs):
        ''' Add a constructor argument '''
        self.ctor_args.append(CtorArg(*args, **kwargs))

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

    def instantiate(self, name):
        ''' How to instantiate ourselves '''
        txt = name + " = "
        txt += "make_" + self.basename.lower() + "("
        txt += '"' + name + '"'
        for arg in self.ctor_args:
            txt += ", " + arg.cname
        txt += ");"
        return txt

    def create(self):
        ''' How to create ourselves '''
        txt = self.basename.lower() + "("
        txt += "sc_module_name name"
        for arg in self.ctor_args:
            txt += ", " + arg.protoform()
        txt += ");"
        return txt

    def decl(self, name):
        ''' How to declare ourselves '''
        return "struct " + self.basename.lower() + " *" + name + ";"

    def emit_pub_hh(self):
        ''' Produce the public .hh file '''
        for arg in self.ctor_args:
            self.sf_pub.fmt(arg.ctype + ";\n")

        self.sf_pub.write("\n")

        self.sf_pub.fmt("struct «lll» *make_«lll»(\n    sc_module_name name")
        for arg in self.ctor_args:
            self.sf_pub.fmt(",\n    " + arg.protoform())
        self.sf_pub.write("\n);\n")

    def emit_hh(self):
        ''' Produce the .hh file '''
        self.sf_hh.write('\n')
        self.sf_hh.fmt('SC_MODULE(«lll»)\n')
        self.sf_hh.fmt('{\n')
        for mbr in self.members:
            self.sf_hh.write('\t' + mbr + "\n")
        for sig in self.signals.values():
            self.sf_hh.fmt('\t' + sig.decl() + "\n")
        for name, child in self.children.items():
            self.sf_hh.fmt('\t' + child.decl(name) + "\n")
        self.sf_hh.write('\n')
        self.sf_hh.fmt('\t' + self.create() + "\n")
        self.sf_hh.fmt('};\n')

    def emit_cc(self):
        ''' Produce the .cc file '''
        self.sf_cc.include(self.sf_pub)
        self.sf_cc.fmt("struct «lll» *make_«lll»(\n    sc_module_name name")
        for arg in self.ctor_args:
            self.sf_cc.fmt(",\n    " + arg.protoform())
        self.sf_cc.write("\n)\n")
        self.sf_cc.write("{\n")
        self.sf_cc.fmt('\treturn new «lll»(name')
        for arg in self.ctor_args:
            self.sf_cc.fmt(", " + arg.cname)
        self.sf_cc.write(');\n}\n')

        self.sf_cc.fmt('''«lll» :: «lll»(\n''')
        self.sf_cc.fmt('''    sc_module_name name''')
        for arg in self.ctor_args:
            self.sf_cc.fmt(',\n    ' + arg.protoform() + "\n")
        self.sf_cc.write('\n) :\n')
        self.sf_cc.fmt('\tsc_module(name)')
        for sig in self.signals.values():
            self.sf_cc.write(",\n\t" + sig.init())
        self.sf_cc.fmt('\n{\n')
        for name, child in self.children.items():
            self.sf_cc.fmt("\t" + child.instantiate(name) + "\n")
        self.sf_cc.fmt('''}\n''')
