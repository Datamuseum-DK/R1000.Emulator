from board import Board

class MEM_Board(Board):

    DFSM0_BITS = (
        "FSM_DONE~",
        "DI_BR_SEL0",
        "DI_BR_SEL1",
        "DI_BR_SEL2",
        "DI_BR_SEL3",
        "DI_NSTATE1",
        "DI_NSTATE2",
        "DI_NSTATE3",
    )

    DFSM1_BITS = (
        "DI_SCAN_SEL0",
        "DI_SCAN_SEL1",
        "DI_SCAN_SEL2",
        "DIAG_XCVR_OE~",
        "DIAG_FLAG",
        "P_TO_S_SEL0",
        "P_TO_S_SEL1",
        "b7",
    )
    DFSM2_BITS = (
        "BUS_PROT~",
        "DIAG_BUZZ_OFF~",
        "D_DRVMEM~",
        "D_DRVTAG~",
        "D_BUS_MODE0",
        "D_BUS_MODE1",
        "D_BUS_MODE2",
        "D_BUS_MODE3",
    )
    DFSM3_BITS = (
        "TRIGGER_SCOPE",
        "D_CNTR_OP0",
        "D_CNTR_OP1",
        "D_CNTR_OP2",
        "D_CNTR_OP3",
        "D_CNTR_OP4",
        "D_EARLY_ABRT~",
        "D_LATE_ABRT~",
    )
    DFSM4_BITS = (
        "DREG_S0",
        "DREG_S1",
        "MAR_MODE0",
        "MAR_MODE1",
        "MAR_MODE2",
        "CNTR8_LD~",
        "D_LOAD_WDR~",
        "D_EN_WDR~",
    )
    DFSM5_BITS = (
        "P_TO_S_S0",
        "P_TO_S_S1",
        "TS_PAR_MODE0",
        "TS_PAR_MODE1",
        "nothing",
        "REFRESH~",
        "D_CMD_EN",
        "FREEZE_EN",
    )
    DFSM6_BITS = (
        "D_DIS_ADR",
        "TRACE_MODE0",
        "TRACE_MODE1",
        "TRACE_MODE2",
        "TRACE_MODE3",
        "COMMAND_OE~",
        "CMD_TRACE_DIR",
        "TAGCOMP_EN~",
    )

    def __init__(self, unit):
        super().__init__("MEM%d" % unit, 14 - unit)

    def Ins_da(self, adr, lines):

        if self.mem[adr+1] == 0x00 and self.mem[adr + 3] == 0x03:
            p = self.mem[adr+2]
            l = ["%02x" % self.mem[x] for x in range(p, p + 9)]
            print("    MAR <= @%02x: " % p + " ".join(l))
            regs = {}
            for i in lines:
                if ".MAR" in i[1]:
                    regs[i[1]] = i
            for i, j in sorted(
                regs.items(),
                key=lambda x: int(x[1][1].split("MAR")[-1])
            ):
                print("    ", j)
            return True
        if self.mem[adr+1] == 0x02 and self.mem[adr + 3] == 0x83:
            p = self.mem[adr+2]
            print("    MAR => @%02x " % p)
            for i in lines:
                if ".MAR" in i[1]:
                    print("    ", i)
            return True

    def DFSM0(self, line):
        self.explines.append(line[:2] + [self.just_bits(line[-1], self.DFSM0_BITS)])

    def DFSM1(self, line):
        self.explines.append(line[:2] + [self.just_bits(line[-1], self.DFSM1_BITS)])

    def DFSM2(self, line):
        self.explines.append(line[:2] + [self.just_bits(line[-1], self.DFSM2_BITS)])

    def DFSM3(self, line):
        self.explines.append(line[:2] + [self.just_bits(line[-1], self.DFSM3_BITS)])

    def DFSM4(self, line):
        self.explines.append(line[:2] + [self.just_bits(line[-1], self.DFSM4_BITS)])

    def DFSM5(self, line):
        self.explines.append(line[:2] + [self.just_bits(line[-1], self.DFSM5_BITS)])

    def DFSM6(self, line):
        self.explines.append(line[:2] + [self.just_bits(line[-1], self.DFSM6_BITS)])
