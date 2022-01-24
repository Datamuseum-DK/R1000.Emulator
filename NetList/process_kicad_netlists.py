#!/usr/bin/env python3

'''
   Turn kicad netlist files into SystemC source code
   =================================================
'''

import sys
import os

import transit

from sexp import SExp

ME = os.path.basename(__file__)

def commit_file(filename):
    ''' Only touch final file if we changed it '''
    try:
        was = open(filename, "r").read()
    except FileNotFoundError:
        was = ""
    now = open(filename + "_", "r").read()
    if was != now:
        print("Changed:", filename)
        os.rename(filename + "_", filename)
    else:
        #print("Same:", filename)
        os.remove(filename + "_")

def pagename_to_sheet(text):
    ''' Convert a sheets name to (our) sheet number '''
    if text == "/":
        return 0
    assert text[:6] == "/Page "
    assert text[-1] == "/"
    return int(text[6:-1], 10)

class Node():
    ''' A `node` from the netlist file '''
    def __init__(self, net, sexp):
        self.net = net
        self.sexp = sexp
        self.refname = sexp[0][0].name[1:-1]
        self.pinno = sexp[1][0].name[1:-1]
        self.component = self.net.board.components[self.refname]
        self.component.connections[self.pinno] = self

    def __str__(self):
        return "_".join((str(self.net), str(self.component), str(self.pinno)))

class Net():
    ''' A `net` from the netlist file '''
    def __init__(self, board, sexp):
        self.board = board
        self.sexp = sexp
        self.name = self.sexp[1][0].name[1:-1]
        self.board.nets[self.name] = self
        self.nodes = []
        for i in sexp.find("node"):
            self.nodes.append(Node(self, i))
        self.sheets = set()
        self.is_plane = self.name in ("PU", "PD")
        for node in self.nodes:
            if node.component.is_plane:
                self.is_plane = True
                self.name = node.component.ref
                continue
            self.sheets.add(node.component.sheet)
        self.sheets = list(self.sheets)
        self.is_local = not self.is_plane and len(self.sheets) == 1
        if self.is_local:
            self.sheets[0].local_nets.append(self)
        self.find_cname()

    def find_cname(self):
        ''' This is gnarly '''
        self.cname = self.name
        if self.cname[0].isdigit():
            self.cname = "z" + self.cname
        if len(self.nodes) == 1 and "unconnected" in self.cname:
            i = self.nodes[0]
            self.cname = "u_" + i.component.name + "_" + i.pinno
            # print("ZZ", self.cname, self.nodes[0])
        for find, replace in (
            ("Net-", ""),
            ("/Page ", "p"),
            ("unconnected-", ""),
            ("(", ""),
            (")", ""),
            ("-", "_"),
            ("/", "_"),
            (".", "_"),
            ("~", "inv"),
        ):
            self.cname = self.cname.replace(find, replace)
        self.bcname = self.cname
        if self.is_plane:
            self.cname = "planes." + self.name
        elif not self.is_local:
            self.cname = self.board.lname + "_globals." + self.cname

    def __str__(self):
        return self.name

    def __lt__(self, other):
        return self.name < other.name

    def decl(self):
        ''' Return a C declaration of this net '''
        text = "\tsc_signal_resolved " + self.bcname + ";\t"
        while len(text.expandtabs()) < 64:
            text += "\t"
        return text + "// " + self.name + "\n"

class Component():
    ''' A `component` from the netlist file '''
    def __init__(self, board, sexp):
        self.board = board
        self.sexp = sexp
        self.ref = sexp[0][0].name[1:-1]
        self.value = sexp.find_first("value")[0].name[1:-1]
        self.board.components[self.ref] = self

        i = self.sexp.find_first("sheetpath.names")
        self.sheet = board.sheets[pagename_to_sheet(i[0].name[1:-1])]
        self.sheet.components[self.ref] = self

        i = self.sexp.find_first("libsource.part")
        self.partname = i[0].name[1:-1]
        self.is_plane = self.partname in ("GF", "GB")
        if self.is_plane:
            self.ref = self.ref[:2] + "%03d" % int(self.ref[2:], 10)
        self.part = self.board.libparts[self.partname]
        self.is_virtual = self.part.is_virtual
        self.connections = {}

        self.location = "x99"
        self.name = "X"
        if self.is_plane:
            self.name = self.ref
        for i in self.sexp.find("property"):
            if i[0][0].name == '"Location"':
                self.location = i[1][0].name[1:-1]
            elif i[0][0].name == '"Name"':
                self.name = i[1][0].name[1:-1]

        self.scm = "SCM_" + self.part.partname.upper()

    def __str__(self):
        return "_".join((str(self.sheet), self.ref, self.partname, self.location, self.name))

    def __lt__(self, other):
        return self.name < other.name

    def hookup(self, file):
        ''' Emit the SystemC code to hook this component up '''
        file.write("\n\n\t// %s\n" % " ".join((self.ref, self.name, self.location, self.partname)))
        for pin in sorted(self.part.pins.values()):
            text = "\t%s.pin%s(" % (self.name, pin.num)
            text += self.connections[pin.num].net.cname
            text += ");"
            while len(text.expandtabs()) < 64:
                text += "\t"
            file.write(text + "// " + str(pin) + "\n")

class Pin():
    ''' A `pin` on a `libpart` from the netlist file '''
    def __init__(self, libpart, sexp):
        self.libpart = libpart
        self.sexp = sexp

        self.num = sexp[0][0].name[1:-1]
        self.name = sexp[1][0].name[1:-1]
        self.type = sexp[2][0].name[1:-1]
        self.libpart.pins[self.num] = self

    def __str__(self):
        return "_".join((str(self.libpart), self.num, self.name, self.type))

    def __lt__(self, other):
        if self.num.isdigit() and other.num.isdigit():
            return int(self.num, 10) < int(other.num, 10)
        if self.num[0] == other.num[0]:
            return int(self.num[1:], 10) < int(other.num[1:], 10)
        return self.num < other.num

class LibPart():
    ''' A `libpart` from the netlist file '''
    def __init__(self, board, sexp):
        self.board = board
        self.sexp = sexp
        i = self.sexp.find_first("part")
        self.partname = i[0].name[1:-1]
        self.is_virtual = self.partname in ("GB", "GF", "Pull_Up", "Pull_Down")
        self.board.libparts[self.partname] = self
        self.pins = {}
        for i in self.sexp.find("pins.pin"):
            Pin(self, i)

    def __str__(self):
        return "_".join((str(self.board), self.partname))

    def include_stmt(self):
        ''' Return include statement for this libpart '''
        if self.is_virtual:
            return ""
        return '#include "%s.hh"' % self.partname

class Sheet():
    ''' A `sheet` from the netlist file '''
    def __init__(self, board, sexp):
        self.board = board
        self.sexp = sexp
        name = sexp.find_first("name")[0].name[1:-1]
        self.page = pagename_to_sheet(name)
        self.board.sheets[self.page] = self
        # print("Sheet", board.name, self.page)
        self.mod_name = board.name.lower() + "_%02d" % self.page
        self.mod_type = "mod_" + self.mod_name
        self.components = {}
        self.local_nets = []

    def __str__(self):
        return self.board.name + "_%d" % self.page

    def substitute(self, text):
        ''' Substitute things into C-source text '''
        for find, replace in (
            ("\t\t|", ""),
            ("mmm", self.mod_name),
            ("ttt", self.mod_type),
        ):
            text = text.replace(find, replace)
        return self.board.substitute(text)

    def filename(self, suffix):
        ''' Return a suitable C filename '''
        return self.board.filename("_%02d" % self.page + suffix)

    def produce_pub_hh(self):
        ''' ... '''
        dfn = self.filename(".pub.hh")
        with open(dfn + "_", "w") as file:
            file.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
            file.write(self.substitute('''
		|struct mod_planes;
		|struct mod_lll_globals;
		|struct mmm;
		|
		|struct ttt *make_ttt(
		|    sc_module_name name,
		|    mod_planes &planes,
		|    mod_lll_globals &lll_globals
		|);
		|'''))
        commit_file(dfn)

    def produce_hh(self):
        ''' ... '''
        dfn = self.filename(".hh")
        with open(dfn + "_", "w") as file:
            file.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
            incls = set(i.part.include_stmt() for i in self.components.values())
            for i in sorted(incls):
                if i:
                    file.write(i + "\n")
            file.write("\n")
            file.write("SC_MODULE(%s)\n" % self.mod_type)
            file.write("{\n")
            for i in sorted(self.local_nets):
                file.write(i.decl())
            file.write("\n")
            for i in sorted(self.components.values()):
                if not i.is_virtual:
                    file.write("\t" + i.scm + " " + i.name + ";\n")
            file.write("\n")
            file.write(self.substitute('''
		|	ttt(
		|	    sc_module_name name,
		|	    mod_planes &planes,
		|	    mod_lll_globals &lll_globals
		|	);
		|};
		|'''))
        commit_file(dfn)

    def produce_cc(self):
        ''' ... '''
        dfn = self.filename(".cc")
        with open(dfn + "_", "w") as file:
            file.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
            file.write(self.substitute('''
		|#include <systemc.h>
		|#include <planes.hh>
		|#include "lll_globals.hh"
		|#include "mmm.hh"
		|#include "mmm.pub.hh"
		|
		|struct ttt *make_ttt(
		|    sc_module_name name,
		|    mod_planes &planes,
		|    mod_lll_globals &lll_globals
		|)
		|{
		|	return new ttt(name, planes, lll_globals);
		|}
		|
		|ttt :: ttt(
		|    sc_module_name name,
		|    mod_planes &planes,
		|    mod_lll_globals &lll_globals
		|) :
		|'''))
            file.write("\tsc_module(name)")
            for i in sorted(self.local_nets):
                file.write(",\n\t" + i.cname + '("' + i.cname + '", sc_logic_1)')
            for i in sorted(self.components.values()):
                if not i.is_virtual:
                    file.write(",\n\t" + i.name + '("' + i.name + '", "' + i.value + '")')
            file.write("\n{\n")
            for i in sorted(self.components.values()):
                if not i.is_virtual:
                    i.hookup(file)
            file.write("}\n")
        commit_file(dfn)

    def produce(self):
        ''' ... '''
        self.produce_pub_hh()
        self.produce_hh()
        self.produce_cc()

class Board():
    ''' A netlist file '''
    def __init__(self, netlist):
        self.dstdir = None
        self.sexp = SExp(None)
        self.sexp.parse(open(netlist).read())
        self.find_board_name()
        print("Board", self.name)
        self.srcs = []

        self.sheets = {}
        for i in self.sexp.find("design.sheet"):
            Sheet(self, i)

        self.libparts = {}
        for i in self.sexp.find("libparts.libpart"):
            LibPart(self, i)

        self.components = {}
        for i in self.sexp.find("components.comp"):
            Component(self, i)

        self.nets = {}
        for i in self.sexp.find("nets.net"):
            Net(self, i)

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

    def filename(self, suffix):
        ''' Return a suitable C filename '''
        if suffix[-3:] == ".cc":
            self.srcs.append(self.name.lower() + suffix)
        return self.dstdir + "/" + self.name.lower() + suffix

    def substitute(self, text):
        ''' Substitute things into C-source text '''
        for find, replace in (
            ("\t\t|", ""),
            ("UUU", self.name),
            ("lll", self.lname),
        ):
            text = text.replace(find, replace)
        return text

    def produce_sheets_h(self):
        ''' ... '''
        dfn = self.filename("_sheets.h")
        with open(dfn + "_", "w") as file:
            file.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
            file.write("#define " + self.name + "_N_SHEETS %d\n" % len(self.sheets))
            file.write("#define " + self.name + "_SHEETS()")
            for sheet in self.sheets.values():
                file.write(" \\\n\tSHEET(" + self.name)
                file.write(", " + self.name.lower())
                file.write(", %2d" % sheet.page)
                file.write(", %02d" % sheet.page + ")")
            file.write("\n")
        commit_file(dfn)

    def produce_hh(self):
        ''' ... '''
        dfn = self.filename(".hh")
        with open(dfn + "_", "w") as file:
            file.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
            file.write(self.substitute('''
		|#include "lll_globals.hh"
		|#include "lll_sheets.h"
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
        commit_file(dfn)

    def produce_pub_hh(self):
        ''' ... '''
        dfn = self.filename(".pub.hh")
        with open(dfn + "_", "w") as file:
            file.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
            file.write(self.substitute('''
		|struct mod_planes;
		|struct mod_lll;
		|
		|struct mod_lll *make_mod_lll(sc_module_name name, mod_planes &planes, const char *how);
		|'''))
        commit_file(dfn)

    def produce_cc(self):
        ''' ... '''
        dfn = self.filename(".cc")
        with open(dfn + "_", "w") as file:
            file.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
            file.write(self.substitute('''
		|#include <systemc.h>
		|#include <planes.hh>
		|#include "lll.hh"
		|#include "lll.pub.hh"
		|
		|'''))
            for sheet in self.sheets.values():
                file.write('#include "%s_%02d.pub.hh"\n' % (self.lname, sheet.page))
            file.write(self.substitute('''
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
            file.write('\t\thow = "%s";\n' % ('+' * len(self.sheets)))
            file.write('\tassert(strlen(how) == %d);\n' % len(self.sheets))
            # ... we could also use the SHEET macro ...
            for sheet in self.sheets.values():
                file.write("\tif (*how++ == '+')\n")
                file.write('\t\t%s = ' % sheet.mod_name)
                file.write(' make_%s(' % sheet.mod_type)
                file.write('"%s", planes, %s_globals);\n' % (sheet.mod_name, self.lname))
            file.write("}\n")
        commit_file(dfn)

    def produce_globals_hh(self):
        ''' ... '''
        dfn = self.filename("_globals.hh")
        with open(dfn + "_", "w") as file:
            file.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
            file.write("\nSC_MODULE(mod_%s_globals)\n" % self.lname)
            file.write("{\n")
            for i in sorted(self.nets.values()):
                if i.is_local or i.is_plane:
                    continue
                file.write(i.decl())
            file.write(self.substitute('''
		|
		|	mod_lll_globals(sc_module_name name);
		|};
		|'''))
        commit_file(dfn)

    def produce_globals_cc(self):
        ''' ... '''
        dfn = self.filename("_globals.cc")
        with open(dfn + "_", "w") as file:
            file.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
            file.write(self.substitute('''
		|#include <systemc.h>
		|#include "lll_globals.hh"
		|#include "lll_globals.pub.hh"
		|
		|struct mod_lll_globals *make_mod_lll_globals(sc_module_name name)
		|{
		|	return new mod_lll_globals(name);
		|}
		|
		|mod_lll_globals :: mod_lll_globals(sc_module_name name) :
		|'''))
            file.write("\tsc_module(name)")
            for i in sorted(self.nets.values()):
                if i.is_local or i.is_plane:
                    continue
                file.write(",\n\t" + i.bcname + '("' + i.bcname + '", sc_logic_1)')
            file.write("\n{\n}\n")
        commit_file(dfn)

    def produce_globals_pub_hh(self):
        ''' ... '''
        dfn = self.filename("_globals.pub.hh")
        with open(dfn + "_", "w") as file:
            file.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
            file.write(self.substitute('''
		|struct mod_lll_globals;
		|
		|struct mod_lll_globals *make_mod_lll_globals(sc_module_name name);
		|'''))
        commit_file(dfn)

    def produce_makefile(self):
        ''' ... '''
        dfn = self.dstdir + "/Makefile"
        with open(dfn + "_", "w") as file:
            file.write("# THIS FILE IS MACHINE GENERATED by " + ME + "\n")
            file.write("SHLIB = " + self.lname + "\n")
            file.write("SHLIB_MAJOR = 1\n")
            file.write("WARNS ?= 3\n")
            file.write("MAN =\n")
            file.write("CXXFLAGS += -I/usr/local/include\n")
            file.write("CXXFLAGS += -I../../Planes\n")
            file.write("CXXFLAGS += -I../../Components\n")
            file.write("CXXFLAGS += -Wall\n")
            file.write("LDFLAGS += -L/usr/local/lib\n")
            file.write("LDFLAGS += -lsystemc\n")
            for i in sorted(self.srcs):
                file.write("SRCS += " + i + "\n")
            file.write(".include <bsd.lib.mk>\n")
        commit_file(dfn)


    def produce(self, dstdir):
        ''' ... '''
        self.dstdir = dstdir
        os.makedirs(self.dstdir, exist_ok=True)
        self.produce_sheets_h()
        self.produce_hh()
        self.produce_pub_hh()
        self.produce_cc()
        self.produce_globals_hh()
        self.produce_globals_pub_hh()
        self.produce_globals_cc()
        for sheet in self.sheets.values():
            sheet.produce()
        self.produce_makefile()
        open(self.dstdir + "/_timestamp", "w").write("\n")

def main():
    ''' ... '''
    try:
        open("NetList/" + ME)
    except FileNotFoundError:
        sys.stderr.write("Must run %s from root of R1000.Emulator project\n" % ME)
        sys.exit(2)

    if len(sys.argv) == 2 and sys.argv[1] == "planes":
        os.makedirs("Planes", exist_ok=True)
        dfn = "Planes/plane_tbl.h"
        transit.make_transit_h(open(dfn + "_", "w"))
        commit_file(dfn)
        sys.exit(0)

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
        if t_new > t_old:
            print("Processing", filename)
            board = Board(filename)
            board.produce(board.name.capitalize() + "/" + branch)
        else:
            print("Skipping", filename, "is older than", speculative_name)

if __name__ == "__main__":
    main()
