from board import Board

class SEQ_Board(Board):

    DUIRG1_BITS = (
        "LOCL_STOP.DIAG~",
        "BUS_OFF.DIAG",
        "+TYP_ON.DIAG~",
        "+VAL_ON.DIAG~",
        "+FIU_ON.DIAG~",
        "+DIAG_MODE~",
        "DIAG.FORCE_CLK",
        "DV_UADR~.DIAG",
    )

    DUIRG2_BITS = (
        "DOUBLE_CLK.FSM",
        "-UADR_PAR.DIAG",
        "+LD_CNT.LOW~",
        "+BUZ_OFF.DIAG",
        "+RND_SEL.DIAG",
        "+WCS_PAR.DIAG",
        "+COUNT.EN~",
        "-WCS.WE",
    )

    DUIRG3_BITS = (
        "ALWAYS.MODE.0",
        "ALWAYS.MODE.1",
        "UIR.MODE.0",
        "UIR.MODE.1",
        "DECODER.MODE.0",
        "DECODER.MODE.1",
        "DG_SEQ.MODE.0",
        "DG_SEQ.MODE.1",
    )

    DUIRG4_BITS = (
        "-DSYNC.FSM",
        "DIAG_STOP",
        "SCAN.MODE.0",
        "SCAN.MODE.1",
        "-CUR_DV~.DIAG",
        "+DEC_WRITE.OE~",
        "-DEC_CS.DIAG",
        "+DEC_WE~.DIAG",
    )

    DUIRG5_BITS = (
        "FSM_DONE~",
        "-SYNC_FREZ.EN",
        "+NOVRAM.WE~",
        "+NOVRAM.CS~",
        "+NOVRAM.STORE~",
        "DEC.0",
        "DEC.1",
        "DEC.2",
    )

    DUIRG6_BITS = (
        "DGRG_TYP.OE~",
        "DGRG_VAL.OE~",
        "ADR_OFF.DIAG~",
        "DFREEZE.FSM~",
        "COUNT.UP",
        "LD_CNT.HIGH~",
        "ADR_ON.DIAG~",
        "PREDECODE.FSM",
    )

    DUIRG7_BITS = (
        "FSM_BANK_SELECT",
        "PARITY_CHK.EN",
        "FLIP_VAL.DRV",
        "DG_SEQ_V.MODE0",
        "DG_SEQ_V.MODE1",
        "LOC_STOP.DIAG2~",
        "b6",
        "STOP_MACH.ME~",
    )

    def __init__(self):
        super().__init__("SEQ", 2)

    def Ins_cb(self, adr, lines):

        if self.mem[adr+1] == 0x31:
            print("    SEQDG.L => @R3 (0x%02x)" % self.mem[0x13])
            return True

    def Ins_da(self, adr, lines):

        if self.mem[adr+1] == 0xa0 and self.mem[adr + 3] == 0x41:
            p = self.mem[adr+2]
            l = ["%02x" % self.mem[x] for x in range(p, p + 6)]
            print("    UIR = @%02x: " % p + " ".join(l))
            regs = {}
            for i in lines:
                if ".UIR" in i[1]:
                    regs[i[1]] = i
            for i, j in sorted(regs.items()):
                print("    ", j)
            return True

        if self.mem[adr+1] == 0xc0 and self.mem[adr + 3] == 0x43:
            p = self.mem[adr+2]
            l = ["%02x" % self.mem[x] for x in range(p, p + 9)]
            print("    SEQCHAIN <= @%02x: " % p + " ".join(l))
            rlist = (
                "RESTRG", "TSVLD", "PAREG0", "LTCHRG",	# DIAG.D4
                "UADR0", "UADR1", "UADR2", "UADR3",	# DIAG.D5
                "UEVNT0", "UEVNT1", "UEVNT2", "UEVNT3",	# DIAG.D6
                "BHREG0", "BHREG1", "MEVNT0", "MEVNT1",	# DIAG.D7
            )
            regs = {}
            for i in lines:
                j = i[1].split(".")[-1]
                if j in rlist:
                    regs[j] = i
            for i in rlist:
                if i in regs:
                    print("    ", regs[i])
            return True

        if self.mem[adr+1] == 0xc2 and self.mem[adr + 3] == 0x36:
            p = self.mem[adr+2]
            print("    SEQCHAIN => @%02x: " % p)
            return True

    def Ins_c4(self, adr, lines):
        if self.mem[adr + 2] == 0x43:
            print("    SEQCHAIN <= #0x%02x" % self.mem[adr + 1])
            return True

    def DUIRG1(self, line):
        self.explines.append(line[:2] + [self.just_bits(line[-1], self.DUIRG1_BITS)])

    def DUIRG2(self, line):
        self.explines.append(line[:2] + [self.just_bits(line[-1], self.DUIRG2_BITS)])

    def DUIRG3(self, line):
        self.explines.append(line[:2] + [self.just_bits(line[-1], self.DUIRG3_BITS)])

    def DUIRG4(self, line):
        self.explines.append(line[:2] + [self.just_bits(line[-1], self.DUIRG4_BITS)])

    def DUIRG5(self, line):
        self.explines.append(line[:2] + [self.just_bits(line[-1], self.DUIRG5_BITS)])

    def DUIRG6(self, line):
        self.explines.append(line[:2] + [self.just_bits(line[-1], self.DUIRG6_BITS)])

    def DUIRG7(self, line):
        self.explines.append(line[:2] + [self.just_bits(line[-1], self.DUIRG7_BITS)])
