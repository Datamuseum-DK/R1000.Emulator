#!/usr/bin/env python3

'''
   Turn kicad netlist files into SystemC source code
   =================================================
'''

import os

from sexp import SExp

from srcfile import SrcFile
from scmod import SC_Mod
from sheet import Sheet
from libpart import LibPart
from model import Model
from net import Net
from bus import BusSchedule

class Board():
    ''' A netlist file '''
    def __init__(self, netlist, branch):
        self.branch = branch
        self.sexp = SExp(None)
        self.sexp.parse(open(netlist).read())
        self.find_board_name()
        print("Board", self.name)
        self.dstdir = self.name.capitalize() + "/" + branch
        self.srcs = []

        self.chf_sheets = SrcFile(self.dstdir + "/" + self.lname + "_sheets.h")
        self.scm_board = self.sc_mod(self.lname + "_board")
        self.scm_globals = self.sc_mod(self.lname + "_globals")

        self.sheets = {}
        for i in self.sexp.find("design.sheet"):
            Sheet(self, i)

        self.libparts = {}
        for i in self.sexp.find("libparts.libpart"):
            LibPart(self, i)

        self.components = {}
        for i in self.sexp.find("components.comp"):
            comp = Model(self, i)

        self.nets = {}
        for i in self.sexp.find("nets.net"):
            Net(self, i)

        for comp in self.components.values():
            comp.post_parse()

        BusSchedule(self)

    def __str__(self):
        return self.name

    def find_board_name(self):
        ''' We dont trust the filename '''
        title = self.sexp.find_first('design.sheet.title_block.title')
        i = title[0].name[1:-1].split()
        assert i[1] == "Main"
        assert i[0].upper() == i[0]
        self.name = i[0]
        self.lname = i[0].lower()

    def sc_mod(self, basename):
        ''' ... '''
        return SC_Mod(self.dstdir + "/" + basename)

    def substitute(self, text):
        ''' Substitute things into C-source text '''
        for find, replace in (
            ("\t\t|", ""),
            ("UUU", self.name),
            ("lll", self.lname),
        ):
            text = text.replace(find, replace)
        return text

    def produce_sheets_h(self, file):
        ''' ... '''
        file.write("#define " + self.name + "_N_SHEETS %d\n" % len(self.sheets))
        file.write("#define " + self.name + "_SHEETS()")
        for sheet in self.sheets.values():
            file.write(" \\\n\tSHEET(" + self.name)
            file.write(", " + self.name.lower())
            file.write(", %2d" % sheet.page)
            file.write(", %02d" % sheet.page + ")")
        file.write("\n")

    def produce_board_pub_hh(self, scm):
        ''' ... '''
        scm.write(self.substitute('''
		|struct mod_planes;
		|struct mod_lll;
		|
		|struct mod_lll *make_mod_lll(sc_module_name name, mod_planes &planes, const char *how);
		|'''))

    def produce_board_hh(self, scm):
        ''' ... '''
        scm.include(self.scm_globals.hh)
        scm.include(self.chf_sheets)
        scm.write(self.substitute('''
		|
		|#define SHEET(upper, lower, nbr, fmt) struct mod_##lower##_##fmt;
		|UUU_SHEETS()
		|#undef SHEET
		|
		|SC_MODULE(mod_lll)
		|{
		|	mod_lll_globals lll_globals;
		|	#define SHEET(upper, lower, nbr, fmt) mod_##lower##_##fmt *lower##_##fmt;
		|	UUU_SHEETS()
		|	#undef SHEET
		|
		|	mod_lll(sc_module_name name, mod_planes &planes, const char *how);
		|};
		|'''))

    def produce_board_cc(self, scm):
        ''' ... '''
        scm.write("#include <systemc.h>\n")
        scm.include("Chassis/planes.hh")
        scm.include(self.scm_board.hh)
        scm.include(self.scm_board.pub)
        scm.write(self.substitute('''
		|
		|'''))
        for sheet in self.sheets.values():
            scm.include(sheet.scm.pub)
        scm.write(self.substitute('''
		|
		|struct mod_lll *make_mod_lll(
		|    sc_module_name name,
		|    mod_planes &planes,
		|    const char *how
		|)
		|{
		|	return new mod_lll(name, planes, how);
		|}
		|
		|mod_lll :: mod_lll(
		|    sc_module_name name,
		|    mod_planes &planes,
		|    const char *how
		|) :
		|	sc_module(name),
		|	lll_globals("lll_globals")
		|{
		|	if (how == NULL)
		|'''))
        scm.write('\t\thow = "%s";\n' % ('+' * len(self.sheets)))
        scm.write('\tassert(strlen(how) == %d);\n' % len(self.sheets))
        # ... we could also use the SHEET macro ...
        for sheet in self.sheets.values():
            scm.write("\tif (*how++ == '+')\n")
            scm.write('\t\t%s = ' % sheet.mod_name)
            scm.write(' make_%s(' % sheet.mod_type)
            scm.write('"%s", planes, %s_globals);\n' % (sheet.mod_name, self.lname))
        scm.write("}\n")

    def produce_globals_pub_hh(self, scm):
        ''' ... '''
        scm.write(self.substitute('''
		|struct mod_lll_globals;
		|
		|struct mod_lll_globals *make_mod_lll_globals(sc_module_name name);
		|'''))

    def produce_globals_hh(self, scm):
        ''' ... '''
        scm.write("\nSC_MODULE(mod_%s_globals)\n" % self.lname)
        scm.write("{\n")
        for i in sorted(self.nets.values()):
            if i.is_local or i.is_plane:
                continue
            scm.write(i.decl())
        scm.write(self.substitute('''
		|
		|	mod_lll_globals(sc_module_name name);
		|};
		|'''))

    def produce_globals_cc(self, scm):
        ''' ... '''
        scm.write("#include <systemc.h>\n")
        scm.include(self.scm_globals.hh)
        scm.include(self.scm_globals.pub)
        scm.write(self.substitute('''
		|
		|struct mod_lll_globals *make_mod_lll_globals(sc_module_name name)
		|{
		|	return new mod_lll_globals(name);
		|}
		|
		|mod_lll_globals :: mod_lll_globals(sc_module_name name) :
		|'''))
        scm.write("\tsc_module(name)")
        for i in sorted(self.nets.values()):
            if i.is_local or i.is_plane:
                continue
            scm.write(",\n\t" + i.bcname + '("' + i.bcname + '", sc_logic_1)')
        scm.write("\n{\n}\n")

    def produce_makefile_inc(self, file):
        ''' ... '''
        self.scm_board.makefile(file)
        self.scm_globals.makefile(file)
        for sheet in self.sheets.values():
            sheet.scm.makefile(file)

    def produce(self):
        ''' ... '''
        os.makedirs(self.dstdir, exist_ok=True)

        self.produce_sheets_h(self.chf_sheets)
        self.chf_sheets.commit()

        self.produce_board_pub_hh(self.scm_board.pub)
        self.produce_board_hh(self.scm_board.hh)
        self.produce_board_cc(self.scm_board.cc)
        self.scm_board.commit()

        self.produce_globals_pub_hh(self.scm_globals.pub)
        self.produce_globals_hh(self.scm_globals.hh)
        self.produce_globals_cc(self.scm_globals.cc)
        self.scm_globals.commit()

        for sheet in self.sheets.values():
            sheet.produce()

        chf = SrcFile(self.dstdir + "/Makefile.inc")
        self.produce_makefile_inc(chf)
        chf.commit()

        open(self.dstdir + "/_timestamp", "w").write("\n")

    def pagename_to_sheet(self, text):
        ''' Convert a sheets name to (our) sheet number '''
        if text == "/":
            return 0
        assert text[:6] == "/Page "
        assert text[-1] == "/"
        return int(text[6:-1], 10)