
from pyreveng import mem, assy, data, listing
from exp_disass import R1kExp

class Board():

    CHAINS = {}

    def __init__(self, name, address):
        self.name = name
        self.address = address
        self.mem = mem.ByteMem(0, 0xff, attr=2)
        self.dlpt = 0x0f # space for length byte
        self.expins = None
        self.explines = []
        self.src = {}
        self.handlers = { }
        self.signals = { }
        self.dbus_state = -1
        self.tref = 0

    def consume(self, line):
        if line[1][-4:] == "PROC":
            self.diproc(line)
            return
        if line[2] == "FIRMWARE":
            return
        line[0] = int(line[0]) - self.tref
        chip = line[1].split(".")[-1]
        func = getattr(self, chip, None)
        if func:
            func(line)
        else:
            self.explines.append(line)

    def finish(self):
        self.one_ins()

    def diproc(self, line):
        if line[2] == "DIAGBUS":
            self.diagbus(line)
            return
        if line[2] == "Instr":
            line[0] = int(line[0]) - self.tref
            if "|MOVX" in line:
                self.explines.append(line)
            elif "|sfr" in line and ( "P2" in line or "P1" in line ):
                self.explines.append(line)
            return
        if line[2] != "Exec":
            return
        self.tref = int(line[0])
        self.one_ins()
        self.expins = line

    def diagbus(self, line):
        # print("DB", self.name, self.dbus_state, line)
        if self.dbus_state == -1:
            if line[5] == "DOWNLOAD" and line[6] == self.name:
                self.dbus_state = 0
            return
        if line[-2] == "payload":
            self.mem[self.dlpt] = int(line[4], 16)
            if self.dlpt == 0x10 + self.mem[0xf]:
                self.disass()
            self.dlpt += 1

    def disass(self):
        cx = R1kExp(board=self.name)
        cx.m.map(self.mem, 0)
        cx.code_base = self.mem[0x10]
        cx.subrs = set()
        cx.disass(cx.code_base)
        print(self.name, "EXPERIMENT")
        print("=" * 80)
        for i in self.mem:
            j = i.render()
            self.src[i.lo] = j
            print("%02x\t" % i.lo, j)
        print("=" * 80)

    def one_ins(self):
        if self.expins:
            adr = int(self.expins[3], 16)
            print("-" * 80)
            print(self.expins[0])
            print("%02x" % adr, self.src.get(adr))
            for i in range(10, len(self.expins), 16):
                print("   %02x: " % (i + 6), " ".join(self.expins[i:i + 16]))
            for n, i in enumerate(self.expins[10:]):
                self.mem[n + 0x10] = int(i, 16)
            handler = getattr(self, "Ins_%02x" % self.mem[adr], None)
            if not handler or not handler(adr, self.explines):
                for i in self.explines:
                    print("   ", i)
        self.expins = None
        self.explines = []

    def update_signal(self, line, signal, state):
        old = self.signals.get(signal)
        if old != state:
            self.explines.append(line[:2] + [signal + (" = 1" if state else " = 0")])
            self.signals[signal] = state

    def explain_bits(self, val, bits):
        text = []
        for i in range(8):
            mask = 0x80 >> i
            if val & mask:
                j = 1
            else:
                j = 0
            if bits[i][0] == '+':
                if not val & mask:
                    text.append(bits[i][1:] + "=0")
            elif bits[i][0] == '-':
                if val & mask:
                    text.append(bits[i][1:] + "=1")
            else:
                text.append(bits[i] + "=%d" % j)

        return ", ".join(text)

    def just_bits(self, val, bits):
        if len(val) < 8:
            v = int(val, 16)
        else:
            v = int(val, 2)
        return bin((1<<8)|v)[3:] + " " + self.explain_bits(v, bits)

    def f374(self, line, *signals):
        if len(line[-1]) < 8:
            v = int(line[-1], 16)
        else:
            v = int(line[-1], 2)
        for i in signals:
            self.update_signal(line, i, v & 0x80)
            v <<= 1
        self.explines.append(line)

    def Ins_5c(self, _adr, _lines):
        print("    END")
        return True

    def do_chain(self, adr, lines, reg, length, tbl, regs):
        mode = self.mem[adr + 1] & 3
        tp = adr + 2
        if mode == 0:
            p = self.mem[adr + 2]
            tp += 1
            q = "<= 0x%02x: "
        elif mode == 1:
            p = self.mem[0x12]
            q = "<= @R2 0x%02x: "
        elif mode == 2:
            p = self.mem[adr + 2]
            tp += 1
            q = "=> @R3 0x%02x: "
            tbl += 1
        elif mode == 3:
            p = self.mem[0x13]
            q = "=> @R3 0x%02x: "
            tbl += 1
        if not mode & 2:
            l = ["%02x" % self.mem[x] for x in range(p, p + length)]
        else:
            l = ["__"] * length
        if self.mem[tp] != tbl:
            msg = "TABLE MISMATCH e:%02x g:%02x" % (tbl, self.mem[tp])
        else:
            msg = ""
        print("   ", reg, q % p, " ".join(l), msg)
        cache = {}
        for i in lines:
            for j in regs:
                if j in i[1]:
                    cache[i[1]] = i
        for i, j in sorted(cache.items()):
            print("    ", j)
        return True

    def Ins_da(self, adr, lines):
        chain_no = self.mem[adr + 1] & 0xfc
        chain = self.CHAINS.get(chain_no)
        if chain is None:
            return False
        return self.do_chain(adr, lines, *chain)

    def Ins_d8(self, _adr, _lines):
        print("    DFSM_RESET")
        return True
