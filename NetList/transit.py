#!/usr/bin/env python3

'''
   Front- and back-plane wiring
   ============================

   The front- and back-planes are mostly, but not entirely parallel wiring:
   The IOC board has a lot of "private" connections to RESHA and other I/O,
   the FIU and MEM32 boards have their own thing going and only the IOC
   and MEM32 know about the ECC lines.

   This file encapsulates the exceptions
'''

import sys

GB = {
    "BP_CLK_2X": "GB294",
    "BP_CLK_2X_": "GB288",
    "BP_PHASE2X": "GB295",

    "CLK_2XE": "GB301",
    "CLK_2XE_": "GB302",
    "CLK_2X": "GB303",
    "CLK_2X_": "GB304",
    "CLK_H1_PHD": "GB305",
    "CLK_H2_PHD": "GB306",
    "CLK_H1E": "GB307",
    "CLK_H2E": "GB308",
    "CLK_H1": "GB309",
    "CLK_H2": "GB310",
    "CLK_Q1": "GB311",
    "CLK_Q2": "GB312",
    "CLK_Q3": "GB313",
    "CLK_Q4": "GB314",

    "RESET": "GB244",
    "CLOCK_DISABLE": "GB296",
    "CLAMP": "GB323",
    "BOARD_HIT0": "GB324",
    "BOARD_HIT1": "GB325",
    "BOARD_HIT2": "GB326",
    "BOARD_HIT3": "GB327",
    "SLOT0": "GB328",
    "SLOT1": "GB329",
}

TRANSIT = {
    "*": {
         "GB293": GB["BP_CLK_2X"],
         "GB287": GB["BP_CLK_2X_"],
    },
    "fiu": {
         "GB268": "*",		# F.PAGE_STATE~0
         "GB271": "*",		# F.PAGE_STATE~1
         "GB264": GB["BOARD_HIT0"],
         "GB265": GB["BOARD_HIT1"],
         "GB266": GB["BOARD_HIT2"],
         "GB267": GB["BOARD_HIT3"],
    },
    "ioc": {
                                                  "GB004": "*",
        "GB005": "*", "GB006": "*", "GB007": "*", "GB008": "*",
                                    "GB013": "*", "GB014": "*",
        "GB015": "*", "GB016": "*", "GB017": "*", "GB018": "*",
        "GB021": "*",               "GB023": "*", "GB024": "*",
        "GB025": "*", "GB026": "*", "GB027": "*", "GB028": "*",
        "GB031": "*", "GB032": "*", "GB033": "*", "GB034": "*",
        "GB035": "*", "GB036": "*", "GB037": "*", "GB038": "*",
        "GB041": "*", "GB042": "*", "GB043": "*", "GB044": "*",
        "GB045": "*", "GB046": "*", "GB047": "*", "GB048": "*",
        "GB051": "*", "GB052": "*", "GB053": "*", "GB054": "*",
        "GB055": "*", "GB056": "*", "GB057": "*", "GB058": "*",
        "GB061": "*", "GB062": "*", "GB063": "*", "GB064": "*",
        "GB065": "*", "GB066": "*", "GB067": "*", "GB068": "*",
        "GB071": "*", "GB072": "*", "GB073": "*",
        "GB075": "*", "GB076": "*", "GB077": "*", "GB078": "*",
                                    "GB083": "*", "GB084": "*",
                                    "GB087": "*", "GB088": "*",
                      "GB092": "*", "GB093": "*", "GB094": "*",

                                    "GB107": "*",
        "GB113": GB["CLAMP"],

                                    "GB137": "*", "GB138": "*",
                                                  "GB144": "*",
        "GB145": "*", "GB146": "*", "GB147": "*", "GB148": "*",
                                    "GB153": "*", "GB154": "*",
        "GB155": "*",
                                    "GB173": "*", "GB174": "*",
        "GB175": "*",               "GB177": "*",
                                                  "GB184": "*",
        "GB185": "*", "GB186": "*", "GB187": "*", "GB188": "*",
        "GB191": "*", "GB192": "*", "GB193": "*", "GB194": "*",
        "GB195": "*", "GB196": "*", "GB197": "*", "GB198": "*",
        "GB201": "*", "GB202": "*", "GB203": "*", "GB204": "*",
        "GB205": "*", "GB206": "*",
        "GB211": "*", "GB212": "*", "GB213": "*", "GB214": "*",
        "GB215": "*",                             "GB218": "*",
        "GB221": "*",               "GB223": "*", "GB224": "*",
        "GB225": "*",               "GB227": "*", "GB228": "*",
        "GB231": "*",               "GB233": "*", "GB234": "*",
        "GB235": "*",               "GB237": "*", "GB238": "*",
        "GB241": "*", "GB242": "*",
        "GB245": "*",               "GB247": "*", "GB248": "*",
        "GB251": "*",               "GB253": "*", "GB254": "*",
        "GB255": "*", "GB256": "*", "GB257": "*", "GB258": "*",

        "GB265": "*", "GB266": "*",
        "GB271": "*", "GB272": "*", "GB273": "*",

        "GF003": "*", "GF007": "*",
        "GF011": "*", "GF015": "*", "GF019": "*",
        "GF023": "*", "GF027": "*",
        "GF031": "*", "GF035": "*", "GF039": "*",
        "GF043": "*", "GF047": "*",
        "GF051": "*", "GF055": "*", "GF059": "*",
        "GF063": "*", "GF067": "*",
        "GF071": "*", "GF075": "*", "GF079": "*",
        "GF083": "*", "GF087": "*",
        "GF091": "*", "GF095": "*", "GF099": "*",
        "GF103": "*", "GF107": "*",
        "GF111": "*", "GF115": "*", "GF119": "*",
        "GF123": "*", "GF127": "*",
        "GF131": "*", "GF167": "*",
        "GF171": "*", "GF175": "*", "GF179": "*",
        "GF183": "*", "GF187": "*",
        "GF191": "*", "GF195": "*", "GF199": "*",
        "GF203": "*", "GF207": "*",
        "GF211": "*", "GF215": "*", "GF219": "*",
        "GF223": "*", "GF227": "*",
        "GF231": "*", "GF235": "*", "GF239": "*",
        "GF243": "*", "GF247": "*",
        "GF251": "*", "GF255": "*", "GF259": "*",
        "GF263": "*", "GF267": "*",
        "GF271": "*", "GF275": "*", "GF279": "*",
        "GF283": "*", "GF287": "*",
        "GF291": "*",
    },
    "mem32": {
        "GB054": "*",
        "GB055": "*",
        "GB056": "*",
        "GB058": "*",
        "GB242": GB["SLOT0"],
        "GB243": GB["SLOT1"],
        "GB264": GB["BOARD_HIT0"],
        "GB265": GB["BOARD_HIT1"],
        "GB266": GB["BOARD_HIT2"],
        "GB267": GB["BOARD_HIT3"],
    },
}

def alloc_transit():
    ''' Allocate dynamic transits '''
    highest = {
	"GF": 320,
	"GB": 319,
    }
    for gates in TRANSIT.values():
        for pin in sorted(gates):
            if gates[pin] != "*":
                highest[pin[:2]] = max(highest[pin[:2]], int(gates[pin][2:], 10) + 1)
    for _brd, gates in sorted(TRANSIT.items()):
        for src, dst in sorted(gates.items()):
            if dst == "*":
                gates[src] = src[:2] + "%03d" % highest[src[:2]]
                highest[src[:2]] += 1
    return highest

PLANE_SIZE = alloc_transit()

def do_transit(board, src):
    ''' Transit one pin for one board '''
    for i in ('*', board.lower()):
        tbl = TRANSIT.get(i, {})
        dst = tbl.get(src)
        if dst:
            return dst
    return src

def make_transit_h(file):
    '''
       Emit a .h file with the transit schedule (as comment)
       and GB_PINS() and GF_PINS() macros or table expansion
    '''
    file.write("/*\n")
    file.write("\n")
    file.write("THIS FILE IS MACHINE GENERATED by transit.py\n")
    file.write("\n")
    boards = ("ioc", "fiu", "seq", "typ", "val", "mem32")
    hdr = ""
    for plane in PLANE_SIZE:
        hdr += "|Plane\t"
        for board in boards:
            hdr += board.upper() + "\t"
    hdr += "|\n"
    line = '-' * len(hdr.expandtabs()) + "\n"
    for pin in range(300):
        if not pin % 50:
            file.write(line)
            file.write(hdr)
            file.write(line)
        text = ""
        for plane, npins in PLANE_SIZE.items():
            if pin >= npins:
                text += "\t\t"
                continue
            pname = plane + "%03d" % pin
            text += "|" + pname + "\t"
            for board in boards:
                dst = do_transit(board, pname)
                if dst != pname:
                    text += dst + "\t"
                else:
                    text += "-\t"
        file.write(text + "|\n")
    file.write(line)
    file.write("\n")
    file.write("Expanded plane dimensions:  " + ", ".join(
        "%s: %d" % (i, j) for i, j in PLANE_SIZE.items()
        ) + "\n"
    )
    file.write("\n")
    file.write("*/\n")
    for plane, npins in PLANE_SIZE.items():
        text = "\n#define %s_PINS()" % plane
        width = 5
        for first in range(0, npins, width):
            file.write(text + " \\\n")
            last = min(first + width, npins)
            text = "\t" + " ".join(
                plane + "PIN(%s%03d, %3d)" % (plane, i, i) for i in range(first, last)
            )
        file.write(text + "\n")

    file.write("\n")
    for k,v in sorted(GB.items()):
        file.write("#define GB_%s %s\n" % (k, v))

if __name__ == "__main__":
    make_transit_h(sys.stdout)
