
import glob

class Summarize():
    def __init__(self, test):

        name = test.split("/")[-1]
        self.test = name[1:].replace(".log", "")

        print("=" * 72)
        print(self.test.upper())
        print("=" * 72)

        self.lines = list(
            (i.split(), i.rstrip()) for i in open(test)
        )

        try:
            self.console = open(test.replace(".log", ".console"), "rb").read()
        except FileNotFoundError:
            self.console = b''

        try:
            self.diproc = list(x for x in open(test.replace(".log", ".diproc")))
        except FileNotFoundError:
            self.diproc = None

        try:
            self.context = list(x for x in open(test.replace(".log", ".context")))
        except FileNotFoundError:
            self.context = None

        # self.missing_chips()
        self.performance()
        self.why()
        self.experiments()

    def missing_chips(self):
        missing = {}
        for flds, line in self.lines:
            if len(flds) != 3:
                continue
            if flds[0] == "MISSING" and flds[2] == "IMPLEMENTATION":
                i = missing.get(flds[1], 0) + 1
                missing[flds[1]] = i
        if missing:
            print()
            print("Missing Chips:")
            for chip, count in sorted(missing.items()):
                print("\t%3d" % count, chip)

    def performance(self):
        silent = True
        for lno, line in enumerate(self.lines):
            if line[1] == "Begin statistics":
                silent = False
                print("")
                print("Statistics:")
            elif line[1] == "End statistics":
                break
            elif not silent:
                print("\t" + line[1])
        if self.diproc:
            t = int(self.diproc[-1].split()[2]) * 1e-9
            print("\t%11.3f s\t\tLast diproc byte code" % t)
        if self.context:
            for line in self.context:
                if "Total" in line:
                    n = int(line.split()[0], 10)
                    print("\t%d\t\tTotal Activations" % n)
                    
           
    def why(self):
        for flds, line in self.lines:
            i = line.split(":")
            if i[0] == "Terminating because":
                print()
                print(i[0] + ":")
                print("\t" + i[1])
                return

    def experiments(self):
        passed = 0
        failed = 0
        hdr = False
        for i in self.console.split(b'\r\n'):
            j = i.split(b'\r\x1b[2K')
            final = j
            if j[-1][-10:] == b'59CPASSED ':
                final = None
                k = j[-1].split(b'\r\x1b[59C')
                passed += len(k) - 1
                continue
            if j[-1][-10:] == b'69CFAILED ':
                final = None
                if not hdr:
                    hdr = True
                    print()
                    print("Failing experiments")
                k = j[-1].split(b'\x07\r\x1b[69C')
                failed += len(k) - 1
                print("\t", k[0].decode("utf-8"))
                for k in j[:-1]:
                    print("\t\t", k.decode("utf-8"))
                continue
        print()
        print("Experiments passed", passed, "failed", failed)
        if final:
            print()
            print("Final Line on Console")
            for k in final:
                print("\t\t" + str([k]))

def main():
    for test in sorted(glob.glob("_*.log")):
        Summarize(test)

if __name__ == "__main__":
    main()
