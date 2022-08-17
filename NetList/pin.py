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
   Pins on components
   ==================
'''

import util

class Pin():

    ''' A `pin` on a `component` '''

    def __init__(self, pinident, pinname, pinrole):
        self.ident = pinident	# Not always numeric!
        self.name = pinname
        self.role = pinrole.replace("+no_connect", "")
        self.pinbus = None
        self.netbus = None
        self.netbuspos = 0
        self.netbusname = ""
        self.update()

    def update(self):
        ''' Things (may) have changed '''

        for i, j in (
            (">", "gt"),
            ("<", "lt"),
            ("=", "eq"),
            ("/", "slash"),
            ("~", "not"),
        ):
            self.name = self.name.replace(i, j)
        if not self.name:
            self.name = "_"
        self.sortkey = util.sortkey(self.name)
        if isinstance(self.sortkey[0], int):
            self.sortkey.insert(0, "_")

    def __repr__(self):
        return "_".join(("Pin", self.ident, self.name, self.role))

    def __lt__(self, other):
        return self.sortkey < other.sortkey

class PinSexp(Pin):

    ''' Create `pin` from netlist-sexp '''

    def __init__(self, sexp):
        super().__init__(
            pinident = sexp[0][0].name,
            pinname = sexp[1][0].name,
            pinrole = sexp[2][0].name,
        )

class PinBus():
    ''' A set of pins named `<prefix>[0…N]` '''

    def __init__(self, busname, low):
        self.name = busname
        self.low = low
        self.pins = []
        self.width = 0

    def __repr__(self):
        return "_".join(("PinBus", self.name, str(self.low), str(len(self.pins))))

    def add_pin(self, pin):
        self.pins.append(pin)
        pin.pinbus = self
        self.width += 1

    def write_extra(self, file, comp):
        file.write("\n")
        sigtype = set()
        direction = set()
        nodes = {}
        for node in sorted(comp):
            if node.pin in self.pins:
                nodes[node.pin] = node
                assert node.pin.role in (
                    "c_input",
                    "c_output",
                    "sc_inout_resolved",
                    "tri_state",
                )
                sigtype.add(node.net.sc_type)
                direction.add(node.pin.role)

        is_bool = len(sigtype) == 1 and "bool" in sigtype

        file.write("\n")
        file.write("#define BUS_%s_WIDTH %d\n" % (self.name, self.width))
        file.write("#define BUS_%s_MASK 0x%xULL\n" % (self.name, (1 << self.width) - 1))

        for pin in self.pins:
            if not pin.netbus:
                continue
            mynodes = pin.netbus.nodes[(comp, pin.pinbus)]
            node0 = mynodes[pin.netbus.nets[0]]
            pin.netbusname = "PINB_" + node0.pin.name

        for i in (
            "c_input",
            "sc_inout_resolved",
        ):
            if i in direction:
                self.write_extra_read(file, nodes)
                self.write_extra_events(file, nodes)
                self.write_extra_sensitive(file, nodes)
                break
        
        for i in (
            "c_output",
            "sc_inout_resolved",
            "tri_state",
        ):
            if i in direction:
                self.write_extra_write(file, nodes)
                if not is_bool:
                    self.write_extra_z(file, nodes)
                break

        file.write("\n")
        file.write("#define BUS_%s_TRACE() \\\n\t\t" % self.name)
        j = []
        for nbr, pin in enumerate(self.pins):
            node = nodes[pin]
            if not pin.netbus:
                j.append("PIN_%s" % pin.name)
            elif node.net.netbus.nets[0] == node.net and node.net.sc_type == "bool":
                j.append("std::bitset<%d>(PINB_%s)" % (len(pin.netbus.nets), pin.name))
            elif node.net.netbus.nets[0] == node.net:
                j.append("PINB_%s" % pin.name)
        file.fmt(' \\\n\t\t<< '.join(j) + '\n')

    def write_extra_sensitive(self, file, nodes):
        file.write("\n")
        file.write("#define BUS_%s_SENSITIVE() \\\n\t" % self.name)
        j = []
        for nbr, pin in enumerate(self.pins):
            node = nodes[pin]
            if not pin.netbus:
                j.append("PIN_%s" % pin.name)
            elif node.net.netbus.nets[0] == node.net:
                j.append("PINB_%s" % pin.name)
        file.write(" << \\\n\t".join(j) + "\n")

    def write_extra_events(self, file, nodes):
        file.write("\n")
        file.write("#define BUS_%s_EVENTS() \\\n\t" % self.name)
        j = []
        for nbr, pin in enumerate(self.pins):
            node = nodes[pin]
            if not pin.netbus:
                j.append("PIN_%s.default_event()" % pin.name)
            elif node.net.netbus.nets[0] == node.net:
                j.append("PINB_%s.default_event()" % pin.name)
        file.write(" | \\\n\t".join(j) + "\n")

    def write_extra_read(self, file, nodes):
        file.write("\n")
        file.write("#define BUS_%s_READ(dstvar) \\\n" % self.name)
        file.write("\tdo { \\\n")
        file.write("\t\t(dstvar) = 0; \\\n")
        if self.width <= 16:
            ctype = "uint16_t"
        elif self.width <= 32:
            ctype = "uint32_t"
        else:
            ctype = "uint64_t"
        for nbr, pin in enumerate(self.pins):
            i = self.width - nbr - 1
            node = nodes[pin]
            if not pin.netbus:
                file.fmt("\t\tif (PIN_%s=>) (dstvar) |= (1ULL << %d); \\\n" % (pin.name, i))
            elif pin.netbus.nets[0].sc_type != "bool":
                file.fmt("\t\tif (IS_H(%s.read()[%d])) (dstvar) |= (1ULL << %d); \\\n" % (
                    pin.netbusname, len(pin.netbus.nets) - (1 + pin.netbusidx), i
                ))
            elif node.net.netbus.nets[0] == node.net:
                shift = self.width - (nbr + len(node.net.netbus))
                file.write("\t\t(dstvar) |= (%s)PINB_%s << %d; \\\n" % (ctype, pin.name, shift))
                
        file.write("\t} while(0)\n")

    def write_extra_write(self, file, nodes):
        file.write("\n")
        file.write("#define BUS_%s_WRITE(dstvar) \\\n" % self.name)
        file.write("\tdo { \\\n")
        for nbr, pin in enumerate(self.pins):
            i = self.width - nbr - 1
            node = nodes[pin]
            if not pin.netbus:
                file.fmt("\t\tPIN_%s<=((dstvar) & (1ULL << %d)); \\\n" % (pin.name, i))
            elif pin.netbus.nets[0] != node.net:
                pass
            elif pin.netbus.nets[0].sc_type != "bool":
                shift = self.width - (nbr + len(node.net.netbus))
                file.write("\t\tPINB_%s = ((dstvar) >> %d) & 0x%x; \\\n" % (pin.name, shift, (1<<len(node.net.netbus)) - 1))
            elif node.net.netbus.nets[0] == node.net:
                shift = self.width - (nbr + len(node.net.netbus))
                file.write("\t\tPINB_%s = ((dstvar) >> %d) & 0x%x; \\\n" % (pin.name, shift, (1<<len(node.net.netbus)) - 1))
        file.write("\t} while(0)\n")

    def write_extra_z(self, file, nodes):
        file.write("\n")
        file.write("#define BUS_%s_Z() \\\n" % self.name)
        file.write("\tdo { \\\n")
        for pin in self.pins:
            node = nodes[pin]
            if not pin.netbus and node.net.sc_type != "bool":
                file.fmt("\t\tPIN_%s = sc_logic_Z; \\\n" % pin.name)
            elif not pin.netbus:
                pass
            elif pin.netbus.nets[0] != node.net:
                pass
            elif pin.netbus.nets[0].sc_type != "bool":
                file.fmt('\t\t%s.write("%s"); \\\n' % (pin.netbusname, "z" * len(pin.netbus.nets)))
        file.write("\t} while(0)\n")
