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
        return "<" + self.kind + "::" + self.ident + "/%d" % self.activations + ">"

def contexts(filename):
    with open(filename, "rb") as file:
        while True:
            try:
                ctx = Context(file)
            except EOFError:
                return
            yield ctx

def main():
    for ctx in contexts(CONTEXT_FILE):
        print(ctx.activations, ctx)

    return
    with open(CONTEXT_FILE, "rb") as file:
        while True:
            ctx = Context(file)
            print(ctx.activations, ctx)
            continue
            prof = struct.unpack("<8192Q", ctx.body)
            nins = 0
            for n, i in enumerate(prof):
                if not i:
                     continue
                print("%04x" % n, "%6d" % i, "%6.2f" % (i*1200 / ctx.activations))
                nins += i
            print("====", "%6d" % nins, "%6.2f" % (nins*1200 / ctx.activations))
   



if __name__ == "__main__":
    main()
