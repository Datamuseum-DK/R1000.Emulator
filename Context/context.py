import struct

CONTEXT_FILE = "/critter/_ctx"

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
        hdr = struct.unpack("<LLQL16s92s", buf)
        if not hdr[0]:
            raise EOFError
        if hdr[0] != 0x6e706c8e:
            raise ValueError
        self.length = hdr[1]
        self.activations = hdr[2]
        self.kind = hdr[4].rstrip(b'\x00').decode("utf-8")
        self.ident = hdr[5].rstrip(b'\x00').decode("utf-8")
        self.body = file.read(self.length - 128)
        return self

    def __repr__(self):
        return self.kind + "::" + self.ident

def contexts(filename):
    with open(filename, "rb") as file:
        while True:
            try:
                ctx = Context(file)
            except EOFError:
                return
            yield ctx

def main():
    nact = 0
    l = []
    p = {}
    for ctx in contexts(CONTEXT_FILE):
        i = ctx.activations
        nact += i
        pg = ctx.ident.split(".")[1]
        if pg not in p:
            p[pg] = 0
        p[pg] += i
        l.append((i, str(ctx)))

    for i, j in p.items():
        l.append((j, i))
        
    for i, ctx in sorted(l):
        print(
            "%12d" % i,
            "%7.3f" % (i / nact),
            ctx
        )
    print("%12d" % nact, "%7.3f" % (nact / nact), "Total")

if __name__ == "__main__":
    main()
