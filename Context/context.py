import sys

import struct

class Context():

    def __init__(self, file=None):
        self.kind = None
        self.ident = None
        self.activations = None
        self.length = None
        self.body = None
        if file:
            self.from_file(file)

    def __lt__(self, other):
        return self.ident < other.ident

    def from_file(self, file):
        buf = file.read(128)
        if len(buf) != 128:
            raise EOFError
        hdr = struct.unpack("<LLQLL104s", buf)
        if not hdr[3]:
            raise EOFError
        if hdr[3] != 0x6e706c8e:
            raise ValueError
        self.length = hdr[4]
        self.activations = hdr[2]
        self.ident, self.kind = hdr[5].rstrip(b'\x00').decode("utf-8").split()
        self.body = file.read(self.length - 128)
        return self

    def __repr__(self):
        return self.kind + "::" + self.ident

def contexts(filename=None):
    if filename is None:
        filename = sys.argv[1]
    with open(filename, "rb") as file:
        while True:
            try:
                ctx = Context(file)
            except EOFError:
                return
            yield ctx

def main():
    nact = 0
    lines = []
    summ = {}
    for ctx in contexts():
        i = ctx.activations
        nact += i
        lines.append((i, str(ctx)))
        j = "page " + ctx.ident.split(".")[1]
        summ[j] = summ.get(j, 0) + i
        j = "kind " + ctx.kind
        summ[j] = summ.get(j, 0) + i

    for i, j in summ.items():
        lines.append((j, i))

    for i, ctx in sorted(lines):
        print(
            "%12d" % i,
            "%7.3f" % (i / nact),
            ctx
        )
    print("%12d" % nact, "%7.3f" % (nact / nact), "Total")

if __name__ == "__main__":
    main()
