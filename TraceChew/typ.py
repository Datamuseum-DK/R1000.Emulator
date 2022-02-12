from board import Board

class TYP_Board(Board):

    CHAINS = {
        0x30: ("WDR", 9, 0x38, [".WDR0", ".WDRP"]),
        0x50: ("UIR", 6, 0x3b, [".URSR"]),
    }

    def __init__(self):
        super().__init__("TYP", 6)

    def DUIRG0(self, line):
        self.f374(
            line,
            "DIAG.HUA0",
            "DIAG.HUA1",
            "DIAG.HUA2",
            "DIAG.HUA3",
            "DIAG.HUA4",
            "DIAG.STATE1",
            "DIAG.STATE2",
            "DIAG.STATE3",
        )

    def DUIRG1(self, line):
        self.f374(
            line,
            "CN.DIAG_ON~",
            "x",
            "BANK_SELECT~",
            "TYPE.DIAG_ON~",
            "FIU.DIAG_ON~",
            "FSM_DONE~",
            "DIAG.WDR.S1~",
            "DIAG.WDR.S0~",
        )

    def DUIRG2(self, line):
        self.f374(
            line,
            "x",
            "A_L.DIAG_OFF~",
            "FORCE_SP_H1~",
            "B_L.DIAG_OFF~",
            "TADR.DIAG_ON~",
            "ACS.DIAG_OFF~",
            "BCS.DIAG_OFF~",
            "DIAG.WDR.EN",
        )

    def DUIRG3(self, line):
        self.f374(
            line,
            "DIAG_STOP",
            "CSA.DIAG.EN~",
            "x",
            "NOVRAM.CS~",
            "DIAG.NO_STOP",
            "COND.DIAG.EN~",
            "UIR.DIAG_OFF",
            "UIR.DIAG_ON~",
        )

    def DUIRG4(self, line):
        self.f374(
            line,
            "B_O.DIAG_ON",
            "C_B.DIAG_ON~",
            "x",
            "A_O.DIAG_ON~",
            "T.T_DIAG_MODE",
            "FREEZE.EN",
            "PAREG.SEL0",
            "PAREG.SEL0",
        )

    def DUIRG5(self, line):
        self.f374(
            line,
            "DIAG_WRITE0",
            "DIAG_WRITE1",
            "CSA.DIAG0",
            "CSA.DIAG1",
            "CSA.DIAG2",
            "DIDEC0",
            "DIDEC1",
            "DIDEC2",
        )

    def DUIRG6(self, line):
        self.f374(
            line,
            "LOOP.DIAG.LD~",
            "LOOP.DIAG.CT~",
            "LOOP.DIAG.EN",
            "DIAG_CNTR.EN~",
            "DIAG_CNTR.LD~",
            "UIR.SEL0~",
            "ADR.DIAG_OFF~",
            "DIAG_UADR.SEL~",
        )
