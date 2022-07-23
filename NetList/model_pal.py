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
   PAL chips
   =========
'''

import os
import glob

from part import PartModel, PartFactory

class PalPin():
    ''' One pin '''
    def __init__(self, nbr):
        self.nbr = nbr
        self.name = None
        self.var = "p%02d" % nbr
        self.pin = "pin%d" % nbr
        self.out = "out%d" % nbr
        self.olmc = None
        self.output = None
        self.inverted = False

    def __lt__(self, other):
        return self.nbr < other.nbr

    def __str__(self):
        if self.inverted:
            return "^" + self.pin
        return self.pin

    def inv(self):
        ''' invert as necessay '''
        if self.inverted:
            return self.var
        return "(!" + self.var + ")"

    def buf(self):
        ''' invert as necessay '''
        if self.inverted:
            return "(!" + self.var + ")"
        return self.var

class PalRow():
    ''' One row of fuses '''
    def __init__(self, up, bits):
        self.up = up
        self.bits = bits
        self.disabled = False
        self.inputs = set()
        self.terms = []

        for nbr, term in enumerate(self.up.palterms):
            fuses = self.bits[nbr * 2:nbr * 2 + 2]
            if min(fuses):
                continue
            if not max(fuses):
                self.disabled = True
                return
            self.inputs.add(term)
            if fuses[1]:
                self.terms.append(term.buf)
            elif fuses[0]:
                self.terms.append(term.inv)

    def cond(self):
        ''' C++ condition string for this row '''
        if self.disabled:
            return "false"
        if not self.terms:
            return "true"
        return " && ".join(i() for i in self.terms)

    def __str__(self):
        return "".join("%d" % i for i in self.bits)

class PalMacroCell():
    ''' Output Logic Macro Cell '''
    def __init__(self, up, pin, regd, rows):
        self.up = up
        self.pin = pin
        pin.olmc = self
        pin.output = self
        self.regd = regd
        self.rows = rows
        self.disabled = None
        self.inputs = set()
        self.output_enable = "true"
        self.xor = False
        self.ands = []
        for row in rows:
            self.inputs |= row.inputs

    def __lt__(self, other):
        return self.pin < other.pin

    def __str__(self):
        return "OLMC:%d" % self.pin.nbr

    def cond(self, nls = ""):
        ''' C++ condition for this output '''
        sep = " ||\n" + nls
        if not self.xor:
            return sep.join("(" + i.cond() + ")" for i in self.ands if not i.disabled)
        return "!(" + sep.join("(" + i.cond() + ")" for i in self.ands if not i.disabled) + ")"


class GAL(PartFactory):
    ''' A generic GAL/PAL chip '''

    def __init__(self, board, ident, palbits):
        super().__init__(board, ident)

        self.palbits = palbits
        self.palpins = {}
        self.palterms = []
        self.palrows = []
        self.palolmc = []
        self.palsig = []
        self.palsyn = None
        self.palac0 = None
        self.palac1 = None
        self.palparse()

        self.palinputs = set()
        self.paloutputs = set()
        for out in self.palolmc:
            if not out.disabled:
                self.paloutputs.add(out.pin)
            self.palinputs |= out.inputs

    def palparse(self):
        ''' ... '''
        assert False

    def extra(self, file):
        file.fmt('''
		|static const sc_logic outs[4] = {
		|	sc_logic_Z,
		|	sc_logic_Z,
		|	sc_logic_0,
		|	sc_logic_1,
		|};
		|static const char traces[5] = "ZZ01";
		|''')

    def sensitive(self):
        for node in self.comp:
            nbr = int(node.pin.ident)
            self.palpins[nbr].name = "PIN_" + node.pin.name

        want = set()
        for out in self.paloutputs:
            if not out.olmc:
                continue
            if out.olmc.regd:
                want.add(self.palpins[1].name + ".pos()")
            else:
                for inp in out.olmc.inputs:
                    if not inp.output:
                        want.add(inp.name)
        yield from sorted(want)

    def state(self, file):
        file.write('\tint job;\n')
        for out in sorted(self.palolmc):
            if not out.disabled:
                file.write('\tint %s;\n' % out.pin.var)

    def doit(self, file):
        ''' The meat of the doit() function '''

        super().doit(file)

        for i in sorted(self.paloutputs):
            file.write("\tassert(0 <= state->%s);\n" % i.var)
            file.write("\tassert(3 >= state->%s);\n" % i.var)

        file.fmt('''
		|
		|	if (state->job) {
		|		TRACE(
		|''')

        for pin in sorted(self.palinputs - self.paloutputs):
            file.fmt('\t\t|\t\t    << %s?\n' % pin.name)
        file.write('\t\t    << " | "\n')
        for out in self.palolmc:
            if out.disabled:
                continue
            pin = out.pin
            if out.output_enable == "true":
                file.write('\t\t    << state->%s\n' % pin.var)
            else:
                file.write('\t\t    << traces[state->%s]\n' % pin.var)
        file.write('\t\t);\n')

        for out in self.palolmc:
            if out.disabled:
                continue
            pin = out.pin
            if "DIBR" in self.name and pin.var in ("p17", "p18"):
                # Ugly hack:  SystemC does not handle two tri-state outputs tied together on
                # a single component, so we need to special-case DIBRPAL
                file.fmt('\t\t|\t\tif (state->%s > 1)\n' % pin.var)
                file.fmt('\t\t|\t\t\tPIN_D0 = outs[state->%s];\n' % pin.var)
            elif out.output_enable == "true":
                file.fmt('\t\t|\t\t%s<=(state->%s);\n' % (pin.name, pin.var))
            else:
                file.fmt('\t\t|\t\t%s = outs[state->%s];\n' % (pin.name, pin.var))
        file.write('\t}\n')

        # Set up the input variables to the equations
        # input pins are read, outputs are set later
        for pin in sorted(self.palinputs):
            out = pin.output
            if out is not None and not out.disabled:
                file.fmt('\t\t|\tbool %s;\n' % pin.var)
            elif self.comp.nodes[pin.name[4:]].net.is_pd():
                file.fmt('\t\t|\tbool %s = false;\n' % pin.var)
            else:
                file.fmt('\t\t|\tbool %s = %s=>;\n' % (pin.var, pin.name))

        # Set up the output variables from the equations
        # registered outputs read their register
        # combinatorial outputs are set later
        for out in self.palolmc:
            if out.disabled:
                pass
            elif out.regd:
                file.write("\tint %s = state->%s & 1;\n" % (out.pin.out, out.pin.var))
            else:
                file.write("\tint %s;\n" % out.pin.out)

        has_combinatorial = False
        for out in self.palolmc:
            if not out.regd and not out.disabled:
                has_combinatorial = True

        if has_combinatorial:
            # Set inputs from outputs
            for pin in sorted(self.palinputs):
                out = pin.output
                if out is not None and not out.disabled:
                    file.write("\t%s = %s;\n" % (pin.var, out.pin.out))

            # Calculate combinatorial outputs
            for out in self.palolmc:
                if not out.regd:
                    out.write_code(file)

        if max(out.regd for out in self.palolmc):

            file.write("\tif (%s.posedge()) {\n" % self.palpins[1].name)

            # Set inputs from outputs, again
            for pin in sorted(self.palinputs):
                out = pin.output
                if out is not None and not out.disabled:
                    file.write("\t\t%s = %s;\n" % (pin.var, out.pin.out))

            # Calculate registered outputs
            for out in self.palolmc:
                if out.regd:
                    out.write_code(file, "\t")

            if has_combinatorial:
                # Set inputs from outputs, again
                for pin in sorted(self.palinputs):
                    out = pin.output
                    if out is not None and not out.disabled:
                        file.write("\t\t%s = %s;\n" % (pin.var, out.pin.out))

                # Recalculate combinatorial outputs
                for out in self.palolmc:
                    if not out.regd:
                        out.write_code(file, "\t")
            file.write("\t}\n")

        file.write("\n")
        for out in self.palolmc:
            if out.disabled:
                continue
            file.write("\tassert(0 <= %s && %s <= 1);\n" % (out.pin.out, out.pin.out))
            if out.output_enable != "true":
                file.write("\tif(%s)\n" % out.output_enable)
                file.write("\t\t%s += 2;\n" % out.pin.out)

        file.write('\n')
        i = []
        j = []
        for out in self.palolmc:
            if out.disabled:
                continue
            i.append("(" + out.pin.out + " != state->" + out.pin.var + ")")
            j.append("\t\tstate->%s = %s;" % (out.pin.var, out.pin.out))
        file.write('\tif (\n\t    %s) {\n' % (" ||\n\t    ".join(i)))
        file.write("\n".join(j) + "\n")
        file.write("\t\tstate->job = 1;\n")
        file.write("\t\tnext_trigger(5, SC_NS);\n")
        file.write('\t}\n')

class PalMacroCell16(PalMacroCell):
    ''' GAL16V8 Output Logic Macro Cell '''

    def __init__(self, up, pin, regd, rows, xor, ac1):
        super().__init__(up, pin, regd, rows)
        self.xor = xor
        self.ac1 = ac1
        if self.ac1:
            self.ands = self.rows[1:]
        else:
            self.ands = self.rows
        self.disabled = min(i.disabled for i in rows)
        if not self.regd:
            self.output_enable = rows[0].cond()
        else:
            p11 = self.up.palpins.get(11)
            if p11 is None:
                p11 = PalPin(11)
                self.up.palpins[11] = p11
            self.output_enable = "!" + p11.var
            self.inputs.add(p11)
        for i in rows:
            self.inputs |= i.inputs

    def dump(self):
        ''' ... '''
        print(self)
        if self.ac1:
            print("\tOE = ", self.rows[0].cond())
        else:
            print("\tOE = pin11")
        cond = self.cond()
        print("\t%s =\n\t   " % self.pin, cond + ";")
        for i in self.rows:
            print(i)

    def write_code_regd_ac01(self, file, pfx):
        ''' Write code for registered mode '''
        assert not self.ac1
        file.write(pfx + self.pin.out + " =\n" + pfx + "    ")
        file.write(self.cond(pfx + "    ") + ";\n")

    def write_code_comb_ac01(self, file, pfx):
        ''' Write code for combinatorial mode '''
        assert self.ac1
        file.write(pfx + self.pin.out + " =\n" + pfx + "    ")
        file.write(self.cond(pfx + "    ") + ";\n")

    def write_code(self, file, pfx=""):
        ''' Write code for this output '''
        if self.disabled:
            return
        pfx = pfx + "\t"
        if self.regd and self.up.palac0:
            self.write_code_regd_ac01(file, pfx)
        else:
            self.write_code_comb_ac01(file, pfx)

    def __str__(self):
        return "OLMC:%d regd %d ac1 %d xor %d" % (self.pin.nbr, self.regd, self.ac1, self.xor)

class GAL16V8(GAL):
    ''' Lattice GAL16V8 '''

    def palparse(self):

        self.palsyn = self.palbits[2192]
        self.palac0 = self.palbits[2193]

        for i in range(1, 20):
            if i != 10:
                self.palpins[i] = PalPin(i)

        if not self.palsyn:
            for nbr in range(8):
                self.palterms.append(self.palpins[2 + nbr])
                self.palterms.append(self.palpins[19 - nbr])
        else:
            for nbr in range(8):
                self.palterms.append(self.palpins[2 + nbr])
                if nbr == 0:
                    self.palterms.append(self.palpins[1])
                elif nbr < 7:
                    self.palterms.append(self.palpins[19 - nbr])
                else:
                    self.palterms.append(self.palpins[11])

        for nbr, start in enumerate(range(0, 2048, 32)):
            self.palrows.append(PalRow(self, self.palbits[start:start+32]))
            assert self.palbits[2128 + nbr] # PTD bit

        for nbr in range(8):
            self.palolmc.append(
                PalMacroCell16(
                    self,
                    self.palpins[19 - nbr],
                    not self.palsyn,
                    self.palrows[nbr * 8: nbr * 8 + 8],
                    not self.palbits[2048 + nbr],
                    self.palbits[2120 + nbr],
                )
            )

        for nbr in range(8):
            i = ''.join("%d" % i for i in self.palbits[2056+nbr*8:2056+8+nbr*8])
            self.palsig.append(i)


class PalMacroCell22(PalMacroCell):
    ''' GAL22V10 Output Logic Macro Cell '''

    def __init__(self, up, pin, rows, s0, s1):
        super().__init__(up, pin, not s1, rows)
        self.s1 = s1
        self.xor = not s0
        if s0 and not s1:
            pin.inverted = True
        self.disabled = min(i.disabled for i in rows)
        self.output_enable = rows[0].cond()
        self.ands = self.rows[1:]
        for i in rows:
            self.inputs |= i.inputs

    def write_code(self, file, pfx=""):
        ''' Write C++ code to evaluate this output '''
        if self.disabled:
            return
        pfx = pfx + "\t"
        dst = self.pin.out
        file.write(pfx + dst + " =\n" + pfx + "    ")
        file.write(self.cond(pfx + "    ") + ";\n")

    def dump(self):
        ''' ... '''
        print(self)
        if self.s1:
            print("\tOE = ", self.rows[0].cond())
        else:
            print("\tOE = pin11")
        cond = self.cond()
        print("\t%s =\n\t   " % self.pin, cond + ";")

    def __str__(self):
        return "OLMC:%d xor %d s1 %d" % (self.pin.nbr, self.xor, self.s1)

class GAL22V10(GAL):

    ''' Lattice GAL22V10 '''

    def palparse(self):

        for i in range(24):
            if i not in (0, 12):
                self.palpins[i] = PalPin(i)

        for nbr in range(11):
            self.palterms.append(self.palpins[1 + nbr])
            if nbr < 10:
                self.palterms.append(self.palpins[23 - nbr])
            else:
                self.palterms.append(self.palpins[13])

        for start in range(0, 5808, 44):
            self.palrows.append(PalRow(self, self.palbits[start:start+44]))

        nxr = 1
        for nbr, rows in enumerate(
            (
                8, 10, 12, 14, 16, 16, 14, 12, 10, 8
            )
        ):
            self.palolmc.append(
                PalMacroCell22(
                    self,
                    self.palpins[23 - nbr],
                    self.palrows[nxr: nxr + rows + 1],
                    self.palbits[5808 + nbr * 2],
                    self.palbits[5809 + nbr * 2],
                )
            )
            nxr += rows + 1

        for nbr in range(8):
            i = ''.join("%d" % i for i in self.palbits[5828+nbr*8:5828+8+nbr*8])
            self.palsig.append(i)

        # Async reset not implemented
        assert self.palrows[0].cond() == "false"
        # Sync preset not implemented
        assert self.palrows[-1].cond() == "false"

class PALModel(PartModel):
    ''' ... '''

    def __init__(self, name, octets):
        self.octets = octets
        self.cls = {
            2194: GAL16V8,
            5892: GAL22V10,
        }.get(len(octets))
        assert self.cls
        super().__init__(name, self.cls)
        self.busable = False

    def create(self, board, ident):
        return self.factory(board, ident, self.octets)

    def assign(self, comp):
        for node in comp:
            # NB: For now make all outputs sc_logic
            if "output" in node.pin.role:
                node.pin.role = "sc_out <sc_logic>"
        super().assign(comp)

def iter_pals():
    ''' Read PAL bitstreams out of firmware.c "library" '''
    with open("Infra/firmware.c") as file:
        sect = []
        for line in file:
            if line[:6] != "static":
                sect.append(line)
                continue
            if "GAL" in sect[0]:
                palname = sect[0].split()[2]
                palname = palname.split('[')[0]
                palname = palname.split('_')[0]
                palname = palname.replace("GAL", "PAL")
                body = bytearray()
                for j in sect[1:]:
                    if j[0] == '}':
                        break
                    j = j.replace(",", " ")
                    j = j.replace("0x", " ")
                    body += bytearray.fromhex(j)
                yield [palname, body]
            sect = [line]

def register(board):
    ''' Register component models '''

    for palname, octets in iter_pals():
        assert len(octets) in (2194, 5892)
        board.add_part(palname, PALModel(palname, octets))
