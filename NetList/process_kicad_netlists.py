#!/usr/bin/env python3

'''
   Turn kicad netlist files into SystemC source code
   =================================================
'''

import sys
import os

import transit

from board import Board
from srcfile import SrcFile

ME = os.path.basename(__file__)

def main():
    ''' ... '''
    try:
        open("NetList/" + os.path.basename(__file__))
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
