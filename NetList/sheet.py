#!/usr/bin/env python3

'''
   Turn kicad netlist files into SystemC source code
   =================================================
'''

class Sheet():
    ''' A `sheet` from the netlist file '''
    def __init__(self, board, sexp):
        self.board = board
        self.sexp = sexp
        name = sexp.find_first("name")[0].name[1:-1]
        self.page = self.board.pagename_to_sheet(name)
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
        incls = set()
        for comp in self.components.values():
            for incl in comp.include_files():
                if incl:
                    incls.add(incl)
        for i in sorted(incls):
            scm.include(i)
        scm.write("\n")
        scm.write("SC_MODULE(%s)\n" % self.mod_type)
        scm.write("{\n")
        for i in sorted(self.local_nets):
            scm.write(i.decl())
        scm.write("\n")
        for i in sorted(self.components.values()):
            if not i.is_virtual:
                i.instance(scm)
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
        for comp in sorted(self.components.values()):
            if not comp.is_virtual:
                comp.initialize(scm)
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
