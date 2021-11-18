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
        hdr = struct.unpack("<LL8s104sQ", buf)
        if not hdr[0]:
            raise EOFError
        if hdr[0] != 0x6e706c8e:
            raise ValueError
        self.length = hdr[1]
        self.kind = hdr[2].rstrip(b'\x00').decode("utf-8")
        self.ident = hdr[3].rstrip(b'\x00').decode("utf-8")
        self.activations = hdr[4]
        self.body = file.read(self.length - 128)
        return self

    def __repr__(self):
        return "<" + self.kind + "::" + self.ident + "/%d" % self.activations + ">"

def main():
    with open(CONTEXT_FILE, "rb") as file:
        while True:
            ctx = Context(file)
            print(ctx)
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
