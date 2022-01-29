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

def pagename_to_sheet(text):
    ''' Convert a sheets name to (our) sheet number '''
    if text == "/":
        return 0
    assert text[:6] == "/Page "
    assert text[-1] == "/"
    return int(text[6:-1], 10)

class SrcFile():
    ''' A C(++) language source file to be written '''
    def __init__(self, filename):
        self.filename = filename
        self.text = []
        self.includes = []

    def write(self, text):
        ''' ... '''
        self.text.append(text)

    def __iter__(self):
        yield self.filename
        for incl in self.includes:
            if isinstance(incl, SrcFile):
                yield from incl
            elif incl == "Chassis/planes.hh":
                yield incl
                yield "Chassis/plane_tbl.h"
            else:
                yield incl

    def include(self, target):
        ''' ... '''
        self.includes.append(target)
        if isinstance(target, SrcFile):
            self.text.append('#include "' + target.filename + '"\n')
        else:
            self.text.append('#include "' + target + '"\n')

    def commit(self):
        ''' ... '''
        now = "".join(self.text)
        try:
            was = open(self.filename, "r").read()
        except FileNotFoundError:
            was = ""
        if was != now:
            print("Changed:", self.filename)
            open(self.filename, "w").write(now)

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
            self.ref = transit.do_transit(self.board.name, self.ref)
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

    def include_file(self):
        ''' Return include statement for this libpart '''
        if self.is_virtual:
            return ""
        return 'Components/' + self.partname + '.hh'

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

        self.scm = self.board.sc_mod(self.mod_name)

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

    def produce_pub_hh(self, scm):
        ''' ... '''
        scm.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
        scm.write(self.substitute('''
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

    def produce_hh(self, scm):
        ''' ... '''
        scm.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
        incls = set(i.part.include_file() for i in self.components.values())
        for i in sorted(incls):
            if i:
                scm.include(i)
        scm.write("\n")
        scm.write("SC_MODULE(%s)\n" % self.mod_type)
        scm.write("{\n")
        for i in sorted(self.local_nets):
            scm.write(i.decl())
        scm.write("\n")
        for i in sorted(self.components.values()):
            if not i.is_virtual:
                scm.write("\t" + i.scm + " " + i.name + ";\n")
        scm.write("\n")
        scm.write(self.substitute('''
		|	ttt(
		|	    sc_module_name name,
		|	    mod_planes &planes,
		|	    mod_lll_globals &lll_globals
		|	);
		|};
		|'''))

    def produce_cc(self, scm):
        ''' ... '''
        scm.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
        scm.write("#include <systemc.h>\n")
        scm.include("Chassis/planes.hh")
        scm.include(self.board.scm_globals.hh)
        scm.include(self.scm.hh)
        scm.include(self.scm.pub)
        scm.write(self.substitute('''
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
        scm.write("\tsc_module(name)")
        for i in sorted(self.local_nets):
            scm.write(",\n\t" + i.cname + '("' + i.cname + '", sc_logic_1)')
        for i in sorted(self.components.values()):
            if not i.is_virtual:
                scm.write(",\n\t" + i.name + '("' + i.name + '", "' + i.value + '")')
        scm.write("\n{\n")
        for i in sorted(self.components.values()):
            if not i.is_virtual:
                i.hookup(scm)
        scm.write("}\n")

    def produce(self):
        ''' ... '''
        self.produce_pub_hh(self.scm.pub)
        self.produce_hh(self.scm.hh)
        self.produce_cc(self.scm.cc)
        self.scm.commit()

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
        file.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
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
        scm.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
        scm.write(self.substitute('''
		|struct mod_planes;
		|struct mod_lll;
		|
		|struct mod_lll *make_mod_lll(sc_module_name name, mod_planes &planes, const char *how);
		|'''))

    def produce_board_hh(self, scm):
        ''' ... '''
        scm.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
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
        scm.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
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
        scm.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
        scm.write(self.substitute('''
		|struct mod_lll_globals;
		|
		|struct mod_lll_globals *make_mod_lll_globals(sc_module_name name);
		|'''))

    def produce_globals_hh(self, scm):
        ''' ... '''
        scm.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
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
        scm.write("/* THIS FILE IS MACHINE GENERATED by " + ME + " */\n")
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
        file.write("# THIS FILE IS MACHINE GENERATED by " + ME + "\n")
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

def main():
    ''' ... '''
    try:
        open("NetList/" + ME)
    except FileNotFoundError:
        sys.stderr.write("Must run %s from root of R1000.Emulator project\n" % ME)
        sys.exit(2)

    os.makedirs("Chassis", exist_ok=True)
    chf = SrcFile("Chassis/plane_tbl.h")
    transit.make_transit_h(chf)
    chf.commit()

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
            board = Board(filename, branch)
            board.produce()
        else:
            print("Skipping", filename, "is older than", speculative_name)

if __name__ == "__main__":
    main()
