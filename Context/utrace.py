#!/usr/bin/env python3

'''
   Dump trace from IOC's microaddress trace RAM(s)
'''

import struct
import context


def main():
    ''' ... '''
    tram = None
    tcnt = None
    for ctx in context.contexts():
        if "ioc_55" not in ctx.ident:
            continue
        if ctx.ident == "IOC.ioc_55.TACNT":
            i = struct.unpack("I", ctx.body[:4])
            tcnt = i[0] & 0x3ff
        elif ctx.ident == "IOC.ioc_55.TRAM":
            tram = struct.unpack("2048H", ctx.body)
    if tcnt is None or tram is None:
        print("NO UTRACE", tcnt is None, tram is None)
        return
    trace = list(tram[tcnt:] + tram[:tcnt])
    while trace[0] == 0:
        trace.pop(0)
    print("UTRACE:")
    for i in trace:
        print("    %04x" % i)

if __name__ == "__main__":
    main()
