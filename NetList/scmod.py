#!/usr/bin/env python3

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
        file.write("\n#" + self.basename + "\n")
        obj = self.basename + ".o"
        file.write("OBJS += " + obj + "\n")
        file.write(obj + ":")
        for incl in sorted(self.cc):
            file.write(" \\\n    " + incl)
        file.write("\n")
        file.write("\t${SC_CC} -o " + obj + " " + self.cc.filename + "\n")
