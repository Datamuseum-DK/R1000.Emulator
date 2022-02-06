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
    Convert JEDEC fuse-maps to SystemC components
    =============================================
'''

import os
import glob

GOOD_SUM = {
    "BUSGAL": 0x04f3,	#
    "CMDGAL": 0x896e,
    "CNTRGAL": 0x2f1b,	#
    "DIBRGAL": 0x57ff,
    "DISTGAL": 0x4f7c,
    "DRADGAL": 0xbd26,	#
    "DRCGAL": 0xa966,	#
    "LUXXGAL": 0x27b8,	#
    "MARGAL": 0xb5ea,
    "MUXEGAL": 0x2318,
    "MUXLGAL": 0x1ba9,
    "PHITGAL": 0x8509,
    "RDRGAL": 0x2526,
    "SETGAL": 0xa744,	#
    "TAGAGAL": 0xacf4,
    "TPARGAL": 0x7ece,
    "TRACGAL": 0xb848,	#
    "TSCGAL": 0xbd7c,	#
    "TSXXGAL": 0x8862,	#
}

def basename(filename):
    ''' return the base-name of this chip '''
    name = filename.split("/")[-1]
    name = name.split("-")[0]
    return name

def checksum(filename, bits):
    ''' calculate checksum '''
    gal = basename(filename)
    good = GOOD_SUM.get(gal, 0)
    csum = 0
    for nbr in range(0, len(bits), 8):
        i = ''.join(reversed(list("%d" % i for i in bits[nbr:nbr+8])))
        csum += int(i, 2)
    csum &= 0xffff
    j = "CSUM %04x" % csum + " CSUM^ %04x" % (csum ^ good) + " %04x" % good
    # print("%-8s" % gal, j, fn)
    return j

class Pin():
    ''' One pin '''
    def __init__(self, nbr):
        self.nbr = nbr
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
        if self.inverted:
            return self.var
        return "(!" + self.var + ")"

    def buf(self):
        if self.inverted:
            return "(!" + self.var + ")"
        return self.var

class Row():
    ''' One row of fuses '''
    def __init__(self, up, bits):
        self.up = up
        self.bits = bits
        self.disabled = False
        self.inputs = set()
        self.terms = []

        for nbr, term in enumerate(self.up.terms):
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

class OLMC():
    ''' Output Logic Macro Cell '''
    def __init__(self, up, pin, regd, rows):
        self.up = up
        self.pin = pin
        pin.output = self
        self.regd = regd
        self.rows = rows
        self.disabled = None
        self.inputs = set()
        self.output_enable = "true"
        self.xor = False
        self.ands = None
        for row in rows:
            self.inputs |= row.inputs

    def __lt__(self, other):
        return self.pin < other.pin

    def __str__(self):
        return "OLMC:%d" % self.pin.nbr

    def cond(self, nls = ""):
        ''' C++ condition for this output '''
        sep = " ||\n" + nls + "\t    "
        if not self.xor:
            return sep.join("(" + i.cond() + ")" for i in self.ands if not i.disabled)
        return "!(" + sep.join("(" + i.cond() + ")" for i in self.ands if not i.disabled) + ")"

    def feedback(self, file):
        ''' Get the feedback value '''
        if not self.regd:
            file.write("\tbool %s = IS_H(%s);\n" % (self.pin.var, self.pin.pin))
        else:
            file.write("\tbool %s = state->%s;\n" % (self.pin.var, self.pin.var))

class GAL():
    ''' Generic Logic Array '''
    def __init__(self, filename):
        self.filename = filename
        self.bits = open(filename, "rb").read()
        checksum(self.filename, self.bits)
        self.pins = {}
        self.terms = []
        self.rows = []
        self.olmc = []
        self.sig = []
        self.syn = None
        self.ac0 = None
        self.parse()

        self.inputs = set()
        self.outputs = set()
        for out in self.olmc:
            if not out.disabled:
                self.outputs.add(out.pin)
            self.inputs |= out.inputs

    def parse(self):
        ''' ... '''
        assert False

    def write_code(self):
        ''' Write code for this GAL '''
        bname = basename(self.filename)
        bname = bname.replace("GAL", "PAL")
        fname = bname + ".cc"
        with open(fname + "_", "w") as file:
            file.write("// Generated from %s by %s\n" % (self.filename, __file__))
            file.write('#include <systemc.h>\n')
            file.write('#include "Chassis/r1000sc.h"\n')
            file.write('#include "Infra/context.h"\n')
            file.write('#include "%s.hh"\n' % bname)

            file.write('\n')
            state = "scm_%s_state" % bname.lower()
            file.write('struct %s {\n' % state)
            file.write('\tstruct ctx ctx;\n')
            for out in sorted(self.olmc):
                if out.regd and not out.disabled:
                    file.write("\tbool %s;\n" % out.pin.var)
            file.write('};\n')

            file.write('\n')
            file.write('void\n')
            file.write('SCM_%s :: loadit(const char *arg)\n' % bname)
            file.write('{\n')
            file.write('\tstate = (struct %s *)\n' % state)
            file.write('\t    CTX_Get("%s", this->name(), sizeof *state);\n' % bname.lower())
            file.write('\tshould_i_trace(this->name(), &state->ctx.do_trace);\n')
            file.write('}\n')

            file.write('\n')
            file.write('void\n')
            file.write('SCM_%s :: doit(void)\n' % bname)
            file.write('{\n')
            file.write('\n')
            file.write('\tstate->ctx.activations++;\n')
            file.write('\n')

            for i in sorted(self.inputs):
                if i.output is None:
                    file.write("\tbool %s = IS_H(%s);\n" % (i.var, i.pin))
                else:
                    i.output.feedback(file)

            file.write("\tbool dotrace = false;\n")
            for out in self.olmc:
                if not out.regd:
                    file.write("\tdotrace = true;\n")
                    out.write_code(file)
            if max(out.regd for out in self.olmc):
                file.write("\tif (pin1.posedge()) {\n")
                file.write("\t\tdotrace = true;\n")
                for out in self.olmc:
                    if out.regd:
                        out.write_code(file)
                file.write("\t}\n")

            file.write("\n")
            for out in self.olmc:
                if out.disabled:
                    continue
                if out.regd:
                    src = "state->%s" % out.pin.var
                else:
                    src = out.pin.var
                if out.output_enable == "true":
                    file.write("\tchar %s = %s ? '1' : '0';\n" % (out.pin.out, src))
                    file.write("\t%s = AS(%s);\n" % (out.pin.pin, src))
                else:
                    file.write("\tchar %s;\n" % (out.pin.out))
                    file.write("\tif(%s) {\n" % out.output_enable)
                    file.write("\t\t%s = %s ? '1' : '0';\n" % (out.pin.out, src))
                    file.write("\t\t%s = AS(%s);\n" % (out.pin.pin, src))
                    file.write("\t} else {\n")
                    file.write("\t\t%s = 'Z';\n" % out.pin.out)
                    file.write("\t\t%s = sc_logic_Z;\n" % out.pin.pin)
                    file.write("\t}\n")

            file.write('\n')
            file.write('\tif (dotrace) {\n')
            file.write('\t\tTRACE(\n')
            for i in sorted(self.inputs - self.outputs):
                file.write("\t\t    <<%s\n" % i.pin)
            file.write('\t\t    << " | "\n')
            for i in sorted(self.outputs):
                file.write("\t\t    <<%s\n" % i.out)
            file.write('\t\t);\n')
            file.write('\t}\n')

            #for out in sorted(self.outputs):
            #    file.write("\t%s = %s;\n" % (out.pin, out.out))
            file.write('}\n')
        try:
            if open(fname).read() != open(fname + "_").read():
                os.rename(fname + "_", fname)
            else:
                os.remove(fname + "_")
        except FileNotFoundError:
            os.rename(fname + "_", fname)

    def dump(self):

        print('=' * len(self.filename))
        print(self.__class__.__name__, self.filename)
        print('=' * len(self.filename))
        print("CHECKSUM", checksum(self.filename, self.bits))
        print("SIG", ",".join(self.sig))
        print("INPUTS", ",".join(str(x) for x in sorted(self.inputs)))
        print("OUTPUTS", ",".join(str(x) for x in sorted(self.outputs)))
        print("INOUT", ",".join(str(x) for x in sorted((self.outputs & self.inputs))))
        print("SYN", self.syn)
        print("AC0", self.ac0)
        print("TERMS", ", ".join("%d" % x.nbr for x in self.terms))
        for out in self.olmc:
            out.dump()

class OLMC16(OLMC):
    ''' GAL16V8 output logic macro cell '''
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
            p11 = self.up.pins.get(11)
            if p11 is None:
                p11 = Pin(11)
                self.up.pins[11] = p11
            self.output_enable = "!" + p11.var
            self.inputs.add(p11)
        for i in rows:
            self.inputs |= i.inputs


    def dump(self):
        print(self)
        if self.ac1:
            print("\tOE = ", self.rows[0].cond())
        else:
            print("\tOE = pin11")
        cond = self.cond()
        print("\t%s =\n\t   " % self.pin, cond + ";")
        for i in self.rows:
            print(i)

    def write_code_regd_ac01(self, file):
        ''' Write code for registered mode '''
        assert not self.ac1
        file.write("\t\tstate->%s =\n\t\t    " % self.pin.var)
        file.write(self.cond("\t") + ";\n")

    def write_code_comb_ac01(self, file):
        ''' Write code for combinatorial mode '''
        assert self.ac1
        file.write("\tbool %s =\n\t    " % self.pin.var)
        file.write(self.cond() + ";\n")

    def write_code(self, file):
        ''' Write code for this output '''
        if self.disabled:
            return
        if self.regd and self.up.ac0:
            self.write_code_regd_ac01(file)
        else:
            self.write_code_comb_ac01(file)

    def __str__(self):
        return "OLMC:%d regd %d ac1 %d xor %d" % (self.pin.nbr, self.regd, self.ac1, self.xor)


class GAL16V8(GAL):

    def parse(self):

        self.syn = self.bits[2192]
        self.ac0 = self.bits[2193]

        for i in range(20):
            if i not in (0, 10):
                self.pins[i] = Pin(i)

        if not self.syn:
            for nbr in range(8):
                self.terms.append(self.pins[2 + nbr])
                self.terms.append(self.pins[19 - nbr])
        else:
            for nbr in range(8):
                self.terms.append(self.pins[2 + nbr])
                if nbr == 0:
                    self.terms.append(self.pins[1])
                elif nbr < 7:
                    self.terms.append(self.pins[19 - nbr])
                else:
                    self.terms.append(self.pins[11])

        for nbr, start in enumerate(range(0, 2048, 32)):
            self.rows.append(Row(self, self.bits[start:start+32]))
            assert self.bits[2128 + nbr] # PTD bit

        for nbr in range(8):
            self.olmc.append(
                OLMC16(
                    self,
                    self.pins[19 - nbr],
                    not self.syn,
                    self.rows[nbr * 8: nbr * 8 + 8],
                    not self.bits[2048 + nbr],
                    self.bits[2120 + nbr],
                )
            )

        for nbr in range(8):
            i = ''.join("%d" % i for i in self.bits[2056+nbr*8:2056+8+nbr*8])
            self.sig.append(i)

class OLMC22(OLMC):
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

    def write_code(self, file):
        ''' Write C++ code to evaluate this output '''
        if self.disabled:
            return
        if self.regd:
            dst = "state->" + self.pin.var
            esp = "\t"
        else:
            dst = "bool " + self.pin.var
            esp = ""

        file.write("\t" + esp + dst + " =\n\t" + esp + "    ")
        file.write(self.cond(esp) + ";\n")

    def dump(self):
        print(self)
        if self.s1:
            print("\tOE = ", self.rows[0].cond())
        else:
            print("\tOE = pin11")
        cond = self.cond()
        print("\t%s =\n\t   " % self.pin, cond + ";")
        if False:
            for i in self.rows:
                if b'\x01' in i.bits:
                    print(i)

    def __str__(self):
        return "OLMC:%d xor %d s1 %d" % (self.pin.nbr, self.xor, self.s1)

class GAL22V10(GAL):

    ''' GAL22V10 output logic macro cell '''

    def parse(self):

        for i in range(24):
            if i not in (0, 12):
                self.pins[i] = Pin(i)

        for nbr in range(11):
            self.terms.append(self.pins[1 + nbr])
            if nbr < 10:
                self.terms.append(self.pins[23 - nbr])
            else:
                self.terms.append(self.pins[13])

        for start in range(0, 5808, 44):
            self.rows.append(Row(self, self.bits[start:start+44]))

        nxr = 1
        for nbr, rows in enumerate(
            (
                8, 10, 12, 14, 16, 16, 14, 12, 10, 8
            )
        ):
            self.olmc.append(
                OLMC22(
                    self,
                    self.pins[23 - nbr],
                    self.rows[nxr: nxr + rows + 1],
                    self.bits[5808 + nbr * 2],
                    self.bits[5809 + nbr * 2],
                )
            )
            nxr += rows + 1

        for nbr in range(8):
            i = ''.join("%d" % i for i in self.bits[5828+nbr*8:5828+8+nbr*8])
            self.sig.append(i)

        # Async reset not implemented
        assert self.rows[0].cond() == "false"
        # Sync preset not implemented
        assert self.rows[-1].cond() == "false"

def do_gal(filename):
    fsiz = os.stat(filename).st_size
    if fsiz == 2194:
        gal = GAL16V8(filename)
        gal.write_code()
        gal.dump()
    elif fsiz == 5892:
        gal = GAL22V10(filename)
        gal.write_code()
        gal.dump()

if __name__ == "__main__":

    for fn in sorted(glob.glob("../_Firmware/*GAL*BIN")):
        do_gal(fn)
