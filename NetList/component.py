#!/usr/bin/env python3

'''
   Turn kicad netlist files into SystemC source code
   =================================================
'''

import transit

class Component():
    ''' A `component` from the netlist file '''

    busable = False

    def __init__(self, board, sexp):
        self.board = board
        self.sexp = sexp
        self.ref = sexp[0][0].name[1:-1]
        self.value = sexp.find_first("value")[0].name[1:-1]
        self.board.components[self.ref] = self
        self.nodes = {}

        i = self.sexp.find_first("sheetpath.names")
        self.sheet = board.sheets[board.pagename_to_sheet(i[0].name[1:-1])]
        self.sheet.components[self.ref] = self

        i = self.sexp.find_first("libsource.part")
        self.partname = i[0].name[1:-1]
        self.is_plane = self.partname in ("GF", "GB")
        if self.is_plane:
            self.ref = self.ref[:2] + "%03d" % int(self.ref[2:], 10)
            self.ref = transit.do_transit(self.board.name, self.ref)
        self.part = self.board.libparts[self.partname]
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

    def add_node(self, node):
        ''' Add a node to this component '''
        self.nodes[node.pinfunction] = node

    def post_parse(self):
        ''' Chance to chew on things between parsing and generation '''

    def include_files(self):
        ''' Register necessary include files '''
        yield self.part.include_file()

    def instance(self, file):
        ''' Emit the local instance of this component '''
        file.write('\t' + self.scm + " " + self.name + ";\n")

    def initialize(self, file):
        ''' Initialize the local instance of this component '''
        file.write(",\n\t" + self.name + '("' + self.name + '", "' + self.value + '")')

    def hookup_pin(self, file, pin_no, pin_num, cmt="", suf=""):
        ''' Text formatting for hooking up a single pin '''
        text = "\t%s.pin%s(" % (self.name + suf, pin_no)
        text += self.connections[pin_num].net.cname
        text += ");"
        if cmt:
            while len(text.expandtabs()) < 64:
                text += "\t"
            text += "// " + cmt
        file.write(text + "\n")

    def hookup(self, file):
        ''' Emit the SystemC code to hook this component up '''
        file.write("\n\n\t// %s\n" % " ".join((self.ref, self.name, self.location, self.partname)))
        for pin in sorted(self.part.pins.values()):
            self.hookup_pin(file, pin.num, pin.num, cmt=str(pin))

class VirtualComponent(Component):
    ''' Components not instantiated in SystemC '''

    def include_files(self):
        if False:
            yield None

    def instance(self, file):
        return

    def initialize(self, file):
        return

    def hookup(self, file):
        return
