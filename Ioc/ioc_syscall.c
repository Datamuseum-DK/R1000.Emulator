
#include "r1000.h"
#include "m68k.h"
#include "ioc.h"
#include "memspace.h"
#include "vend.h"
#include "vsb.h"

static struct vsb *syscall_vsb;

static void
ioc_dump_regs(struct vsb *vsb)
{
	VSB_printf(vsb, "D0 = %08x  D4 = %08x   A0 = %08x  A4 = %08x\n",
	    m68k_get_reg(NULL, M68K_REG_D0), m68k_get_reg(NULL, M68K_REG_D4),
	    m68k_get_reg(NULL, M68K_REG_A0), m68k_get_reg(NULL, M68K_REG_A4)
	);

	VSB_printf(vsb, "D1 = %08x  D5 = %08x   A1 = %08x  A5 = %08x\n",
	    m68k_get_reg(NULL, M68K_REG_D1), m68k_get_reg(NULL, M68K_REG_D5),
	    m68k_get_reg(NULL, M68K_REG_A1), m68k_get_reg(NULL, M68K_REG_A5)
	);

	VSB_printf(vsb, "D2 = %08x  D6 = %08x   A2 = %08x  A6 = %08x\n",
	    m68k_get_reg(NULL, M68K_REG_D2), m68k_get_reg(NULL, M68K_REG_D6),
	    m68k_get_reg(NULL, M68K_REG_A2), m68k_get_reg(NULL, M68K_REG_A6)
	);

	VSB_printf(vsb, "D3 = %08x  D7 = %08x   A3 = %08x  A7 = %08x\n",
	    m68k_get_reg(NULL, M68K_REG_D3), m68k_get_reg(NULL, M68K_REG_D7),
	    m68k_get_reg(NULL, M68K_REG_A3), m68k_get_reg(NULL, M68K_REG_A7)
	);

}

void
ioc_dump_registers(unsigned lvl)
{

	if (fd_trace < 0 || !(do_trace & lvl))
		return;
	if (syscall_vsb == NULL)
		syscall_vsb = VSB_new_auto();
	AN(syscall_vsb);
	VSB_clear(syscall_vsb);
	VSB_printf(syscall_vsb, "%12jd Registers\n", simclock);
	ioc_dump_regs(syscall_vsb);
	AZ(VSB_finish(syscall_vsb));
	VSB_tofile(syscall_vsb, fd_trace);
}

static void
dump_text(struct vsb *vsb, unsigned adr, unsigned len)
{
	unsigned u, c;

	VSB_cat(vsb, "'");
	for (u = adr; u < adr + len; u++) {
		c = m68k_debug_read_memory_8(u);
		if (0x20 <= c && c <= 0x7e)
			VSB_putc(vsb, c);
		else if (c == 0x0a)
			VSB_cat(vsb, "␊");
		else if (c == 0x0d)
			VSB_cat(vsb, "␍");
		else if (c == 0x1b)
			VSB_cat(vsb, "␛");
		else
			VSB_cat(vsb, "␥");
	}
	VSB_cat(vsb, "'");
}

static void
dump_string(struct vsb *vsb, unsigned adr)
{
	unsigned u;

	u = vbe32dec(ram_space + adr - 4);
	VSB_printf(vsb, "-4={0x%x} ", u);
	u = vbe16dec(ram_space + adr);
	VSB_printf(vsb, "len=0x%x ", u);
	dump_text(vsb, adr + 2, u);
}

static void
dump_10204(struct vsb *vsb, unsigned a7)
{
	unsigned u;

	VSB_printf(vsb, " 0x%x", vbe16dec(ram_space + a7 + 4));
	VSB_printf(vsb, " dst=0x%08x", vbe32dec(ram_space + a7 + 6));
	u = vbe32dec(ram_space + a7 + 10);
	VSB_printf(vsb, " ptr=0x%08x {", u);
	VSB_printf(vsb, " 0x%04x", vbe16dec(ram_space + u));
	VSB_printf(vsb, " 0x%x", vbe16dec(ram_space + u + 2));
	VSB_printf(vsb, " 0x%x", vbe16dec(ram_space + u + 4));
	VSB_printf(vsb, " 0x%x", vbe16dec(ram_space + u + 6));
	VSB_printf(vsb, " 0x%x", vbe16dec(ram_space + u + 8));
	VSB_printf(vsb, " 0x%x", vbe16dec(ram_space + u + 10));
	VSB_printf(vsb, " c=0x%04x", vbe16dec(ram_space + u + 12));
	VSB_printf(vsb, " h=0x%02x", ram_space[ u + 14]);
	VSB_printf(vsb, " s=0x%02x", ram_space[u + 15]);
	VSB_printf(vsb, "}\n");
	ioc_dump_regs(syscall_vsb);
}

void
ioc_trace_syscall(unsigned pc)
{
	unsigned a7, u;

	if (fd_trace < 0 || !(do_trace & TRACE_SYSCALL))
		return;
	if (syscall_vsb == NULL)
		syscall_vsb = VSB_new_auto();
	AN(syscall_vsb);
	VSB_clear(syscall_vsb);
	VSB_printf(syscall_vsb, "%12jd Syscall 0x%x ", simclock, pc);

	a7 =  m68k_get_reg(NULL, M68K_REG_A7);
	if (0x10460 <= pc && pc <= 0x104c0)
		return;
	switch (pc) {
	case 0x10204: dump_10204(syscall_vsb, a7); break;
	case 0x10206: return;
	case 0x1020a: return;
	case 0x1022a: return;
	case 0x10238: return;
	case 0x102c4: return;
	case 0x102c8: return;
	case 0x102cc: return;
	case 0x102d0: return;
	case 0x102d4: return;
	case 0x102e4: return;
	case 0x10310: return;
	case 0x10380: return;
	case 0x103d0: return;
	case 0x103d8: return;
	case 0x103e0: return;
	case 0x103e4: return;
	case 0x10460: return;
	case 0x10466: return;
	case 0x1046c: return;
	case 0x10472: return;
	case 0x10478: return;
	case 0x1047e: return;
	case 0x10484: return;
	case 0x10568: return;
	case 0x10da4: return;
	default:
		VSB_cat(syscall_vsb, "\n");
		ioc_dump_regs(syscall_vsb);
		hexdump(syscall_vsb, ram_space + a7, 0x80, a7);
		u = vbe32dec(ram_space + a7 + 4);
		if (0x10000 < u && u < 0x80000)
			hexdump(syscall_vsb, ram_space + u, 0x80, u);
		u = vbe32dec(ram_space + a7 + 8);
		if (0x10000 < u && u < 0x80000)
			hexdump(syscall_vsb, ram_space + u, 0x80, u);
		u = vbe32dec(ram_space + a7 + 12);
		if (0x10000 < u && u < 0x80000)
			hexdump(syscall_vsb, ram_space + u, 0x80, u);
		break;
	}
	AZ(VSB_finish(syscall_vsb));
	VSB_tofile(syscall_vsb, fd_trace);
}

struct syscall {
	const char	*name;
	unsigned	lo;
	unsigned	hi;
	unsigned	no_trace;
	unsigned	dump;
	const char	*call;
	const char	*ret;
};

static void v_matchproto_(mem_event_f)
sc_peg(void *priv, const char *what, unsigned adr, unsigned val,
    unsigned width, unsigned peg)
{
	struct syscall *sc = priv;
	const char *which;
	unsigned a7, sp, u, v, w;

	(void)val;
	(void)what;
	(void)width;
	(void)peg;

	if (ioc_fc != 2 && ioc_fc != 6)
		return;

	if (syscall_vsb == NULL)
		syscall_vsb = VSB_new_auto();
	AN(syscall_vsb);
	VSB_clear(syscall_vsb);
	VSB_printf(syscall_vsb, "%12jd ", simclock);

	a7 =  m68k_get_reg(NULL, M68K_REG_A7);
	if (adr == sc->lo) {
		VSB_printf(syscall_vsb, "CALL %s(%s)\n", sc->name, sc->call);
		which = sc->call;
	} else if (adr == sc->hi) {
		VSB_printf(syscall_vsb, "RETURN %s(%s)\n", sc->name, sc->ret);
		which = sc->ret;
	} else {
		WRONG();
	}
	sp = a7 + 4;
	for(; *which != '\0'; which += 2) {
		if (which[0] == 'D' && which[1] == '0') {
			VSB_printf(syscall_vsb, "\t\tD0: 0x%x\n", m68k_get_reg(NULL, M68K_REG_D0));
			continue;
		}
		if (which[0] == 'D' && which[1] == '2') {
			VSB_printf(syscall_vsb, "\t\tD2: 0x%x\n", m68k_get_reg(NULL, M68K_REG_D2));
			continue;
		}
		if (which[0] == 'A' && which[1] == '0') {
			u = m68k_get_reg(NULL, M68K_REG_A0);
			VSB_printf(syscall_vsb, "\t\tA0: ");
			if (u < 0x80000)
				hexdump(syscall_vsb, ram_space + u, 0x10, u);
			else
				VSB_printf(syscall_vsb, " 0x%x\n", u);
			continue;
		}
		if (which[0] == 'A' && which[1] == '1') {
			u = m68k_get_reg(NULL, M68K_REG_A1);
			VSB_printf(syscall_vsb, "\t\tA1: ");
			if (u < 0x80000)
				hexdump(syscall_vsb, ram_space + u, 0x10, u);
			else
				VSB_printf(syscall_vsb, " 0x%x\n", u);
			continue;
		}
		if (which[0] == 'A' && which[1] == '2') {
			u = m68k_get_reg(NULL, M68K_REG_A2);
			VSB_printf(syscall_vsb, "\t\tA2: ");
			if (u < 0x80000)
				hexdump(syscall_vsb, ram_space + u, 0x10, u);
			else
				VSB_printf(syscall_vsb, " 0x%x\n", u);
			continue;
		}
		if (which[0] == 'i' && which[1] == 'l') {
			// Ignore 32 bits on stack
			sp += 4;
			continue;
		}
		if (which[0] == 'i' && which[1] == 'w') {
			// Ignore 16 bits on stack
			sp += 2;
			continue;
		}
		if (which[0] == 's' && which[1] == 'L') {
			// 32 bit long word on stack
			u = m68k_debug_read_memory_32(sp);
			VSB_printf(syscall_vsb, "\t\tLong: 0x%x\n", u);
			sp += 4;
			continue;
		}
		if (which[0] == 's' && which[1] == 'W') {
			// 16 bit word on stack
			u = m68k_debug_read_memory_16(sp);
			VSB_printf(syscall_vsb, "\t\tWord: 0x%x\n", u);
			sp += 2;
			continue;
		}
		if (which[0] == 's' && which[1] == 'B') {
			// 8 bit byte on stack
			u = m68k_debug_read_memory_8(sp);
			VSB_printf(syscall_vsb, "\t\tByte: 0x%x\n", u);
			sp += 2;
			continue;
		}
		if (which[0] == 's' && which[1] == 'E') {
			// 32 bit experiment pointer on stack
			u = m68k_debug_read_memory_32(sp);
			if (u && u < 0x80000) {
				VSB_printf(syscall_vsb, "\t\tExperiment:\n");
				hexdump(syscall_vsb, ram_space + u, 0x40, u);
			} else {
				VSB_printf(syscall_vsb, "\t\tExperiment: 0x%x\n", u);
			}
			sp += 4;
			continue;
		}
		if (which[0] == 's' && which[1] == 'P') {
			// 32 bit pointer on stack
			u = m68k_debug_read_memory_32(sp);
			VSB_printf(syscall_vsb, "\t\tPointer: ");
			if (u && u < 0x80000)
				hexdump(syscall_vsb, ram_space + u, 0x10, u);
			else
				VSB_printf(syscall_vsb, "0x%x\n", u);
			sp += 4;
			continue;
		}
		if (which[0] == 's' && which[1] == 'q') {
			// 32 bit pointer to pointer on stack
			u = m68k_debug_read_memory_32(sp);
			VSB_printf(syscall_vsb, "\t\tPointer: 0x%x", u);
			u = m68k_debug_read_memory_32(u);
			VSB_printf(syscall_vsb, " *-> 0x%x\n", u);
			sp += 4;
			continue;
		}
		if (which[0] == 's' && which[1] == 'Q') {
			// 32 bit pointer to pointer on stack
			u = m68k_debug_read_memory_32(sp);
			VSB_printf(syscall_vsb, "\t\tPointer: 0x%x", u);
			u = m68k_debug_read_memory_32(u);
			VSB_printf(syscall_vsb, " *-> 0x%x ", u);
			if (u)
				hexdump(syscall_vsb, ram_space + u, 0x40, u);
			else
				VSB_cat(syscall_vsb, "\n");
			sp += 4;
			continue;
		}
		if (which[0] == 's' && which[1] == 'S') {
			// 32 bit pointer to string on stack
			u = m68k_debug_read_memory_32(sp);
			VSB_printf(syscall_vsb, "\t\tString: 0x%x ", u);
			if (u)
				dump_string(syscall_vsb, u);
			VSB_cat(syscall_vsb, "\n");
			sp += 4;
			continue;
		}
		if (which[0] == 'x' && which[1] == '0') {
			// 32 bit pointer to string on stack
			// Length is second previous arg on stack
			v = m68k_debug_read_memory_16(sp - 4);
			u = m68k_debug_read_memory_32(sp);
			VSB_printf(syscall_vsb, "\t\tText[0x%x]: 0x%x ", v, u);
			dump_text(syscall_vsb, u, v);
			VSB_cat(syscall_vsb, "\n");
			sp += 4;
			continue;
		}
		if (which[0] == 'x' && which[1] == '1') {
			// A2 -> string, lengtn in D1
			// Length is second previous arg on stack
			u = m68k_get_reg(NULL, M68K_REG_A2);
			v = m68k_get_reg(NULL, M68K_REG_D1);
			VSB_printf(syscall_vsb, "\t\tText[0x%x]: 0x%x ", v, u);
			dump_text(syscall_vsb, u, v);
			VSB_cat(syscall_vsb, "\n");
			continue;
		}
		if (which[0] == 'x' && which[1] == '2') {
			// 10568 magic
			v = m68k_get_reg(NULL, M68K_REG_A7);
			u = m68k_debug_read_memory_32(v);
			VSB_printf(syscall_vsb, "\t\t@ret\n");
			hexdump(syscall_vsb, ram_space + u, 0x40, u);
			w = m68k_debug_read_memory_16(u);
			VSB_printf(syscall_vsb, "\t\t@0.W: 0x%04x\n", w);
			w = m68k_debug_read_memory_8(u + 2);
			VSB_printf(syscall_vsb, "\t\t@2.B: 0x%02x\n", w);
			VSB_printf(syscall_vsb, "\t\t@3.S:");
			dump_text(syscall_vsb, u + 3, w);
			VSB_printf(syscall_vsb, "\n");
			hexdump(syscall_vsb, ram_space + v + w, 0x40, v+w);
			continue;
		}
		if (which[0] == 'x' && which[1] == '3') {
			hexdump(syscall_vsb, ram_space + 0x150c, 0x100, 0x150c);
			continue;
		}
		WRONG();
	}
	if (sc->dump) {
		VSB_printf(syscall_vsb, "PC = 0x%x\n", adr);
		ioc_dump_regs(syscall_vsb);
		hexdump(syscall_vsb, ram_space + a7, 0x80, a7);
	}
	AZ(VSB_finish(syscall_vsb));
	VSB_tofile(syscall_vsb, fd_trace);
}

static struct syscall syscalls[] = {
	{ ">CONSOLE.3",		0x02374, 0x023de, 0x023e0, 0, "x1", ""},
	{ ">CONSOLE.2",		0x028dc, 0,	  0x02978, 0, "", ""},
	{ ">CONSOLE.0",		0x02978, 0,	  0x029a6, 0, "", ""},
	{ ">CONSOLE.1",		0x02992, 0,	  0x02ab0, 0, "", ""},
	{ ">CONSOLE",		0x02ab0, 0,	  0x02ad2, 0, "", ""},
	{ "DiagBusResponse",	0x0362c, 0x036a8, 0,	   1, "D2", "x3"},
	// { "IS_IDLE?",		0x03638, 0,	  0x0364a, 0, "", ""},
	{ "DO_KC15_DiagBus",	0x0374c, 0x037b0, 0,       0, "D0A0", "D0"},
	{ "_CHS9_LBA10",	0x04b20, 0,	  0x04b82, 0, "", ""},
	{ "_SCSID.0",		0x05502, 0,	  0x05556, 0, "", ""},
	{ "_DISPATCH_KERNCALL",	0x08370, 0,	  0x0838c, 0, "", ""},
	{ "KC_03_WAIT_DISK",	0x08536, 0x8562,  0x8564,  0, "", ""},
	{ "KC_1C",		0x089aa, 0,	  0x089ee, 0, "", ""},
	{ "DEFDMAMAP",		0x08e12, 0,	  0x08eb0, 0, "D0", ""},
	{ "DEFXXMAP",		0x09cee, 0x9d30,  0x09d32, 0, "", ""},
	{ ">PIT.0",		0x09d6e, 0x09d8c, 0x09d8e, 0, "", ""},
	{ ">PIT.1",		0x09d8e, 0x09dc2, 0x09dc4, 0, "", ""},
	{ "ArmTimeout",		0x09dc4, 0x09dfe, 0,       0, "D0A2", ""},
	{ "CancelTimeout",	0x09e00, 0x09e2e, 0,       0, "A2", ""},
	{ ">PIT",		0x09e30, 0x09e68, 0x09e6a, 0, "", ""},
	{ "$IDLE",		0x09e74, 0x09f04, 0x09f06, 0, "", ""},
	{ "KC15_DiagBus",	0x1022a, 0,	  0,	   1, "sPsWsP", "sP,sW,SP"},
	{ "memcpy_protected",	0x10238, 0,	  0,       0, "sWsPsL", ""},
	{ "LINK",		0x103b0, 0,		   0,	    1, "sPsWsSsS", ""},
	{ "INIT.PROGRAM",	0x10656, 0,	  0x10704, 0, "", "" },
	{ "MALLOC",		0x10856, 0x108b8, 0x108de, 0, "sL", "ilsq" },
	{ "FREE",		0x108fa, 0x109fc, 0x109fc, 0, "sLsq", "" },
	{ "ALLOC_STR",		0x10cfa, 0x10d34, 0x10d34, 0, "", "sq" },
	{ "FILL_STR",		0x10da4, 0x10e60, 0x10e60, 0, "sWsWx0", "iwiwilsS"},
	{ "AppendChar",		0x10d66, 0x10da2, 0x10da4, 0, "sBsS", "sBsS" },
	{ "StringEqual",	0x10e62, 0x10ed8, 0x10eda, 0, "sSsS", "ililsB" },
	{ "StringDup",		0x10eda, 0x10f2a, 0x10f2c, 0, "sSsL", "sSsS" },
	{ "StringCat",		0x10f2c, 0x10fc8, 0x10fc8, 0, "sSsSil", "ililsS"},
	{ "StringCat2",		0x10fca, 0x10ffe, 0x11000, 0, "sSsSsS", "ililsS" },
	{ "LongInt2String",	0x110c0, 0x111cc, 0x111ce, 0, "sL", "ilsS" },
	{ "String2LongInt",	0x1127c, 0x113ae, 0x113b0, 0, "sPsPsS", "sPsPsL"},
	{ "ToUpper",		0x113b0, 0x11424, 0x11426, 0, "sS", "sS"},
	{ "Str2Int",		0x11a36, 0x11ab2, 0,	   0, "sPsLsLsPsS", "sPsLsLsPsS" },
	{ "MONTH",		0x11ab4, 0x11b82, 0,	   0, "sPsPsS", "sPsPsS"},
	{ "ConvertTimestamp",	0x11b84, 0x11d02, 0x11d04, 0, "sPsPsS", "sPsPsS"},
	{ "DiskIO",		0x127c4, 0x12992, 0x12994, 0, "sPsPsLsWsB", "sPsPsLsWsB"},
	{ "NameI",		0x13718, 0x138b2, 0x138b4, 0, "sPsP", "sPsP" },
	{ "OPEN?",		0x138b4, 0x13a6e, 0,	   0, "sPsPsLsBsBsS", "sQsPsLsBsBsS"},
	{ "RW1?",		0x13ae6, 0x13bb6, 0,	   0, "sPsPsBsW", "sPsPsBsW"},
	{ "RW2?",		0x13bb8, 0x13c88, 0,	   0, "sPsPsBsW", "sPsPsBsW"},
	{ "RW3?",		0x13c8a, 0x13e5a, 0,	   1, "sPsPsBsWsB", "sPsPsBsWsB"},
	{ "XXX1",		0x144e6, 0x1473e, 0,	   1, "sPsB", "sPsB" },
	{ "PopProgram",		0x14e18, 0x14f66, 0,	   1, "sLsB", ""},
	{ "WR_CON_C",		0x15210, 0,	  0x15286, 0, "sB", ""},
	{ "ReadChar",		0x15286, 0x15390, 0x15392, 0, "", "sB"},
	{ "WriteConsole",	0x15392, 0x15408, 0x15408, 0, "sS", ""},
	{ "WriteConsoleCRLF",	0x1540a, 0x154ae, 0x154b0, 0, "", ""},
	{ "WriteLnConsole",	0x154b0, 0x154f4, 0x154f4, 0, "sS", ""},
	{ "AskConsoleString",	0x15694, 0x158be, 0x158c0, 0, "sS", "ilsS"},
	{ "ReadDir",		0x17d1a, 0x17e94, 0,	   1, "", ""},
	{ "fs_10460",		0x1866c, 0x18b26, 0,	   1, "sPsPsL", "sPsPsP"},
	{ "fs_10466",		0x18b28, 0x18b84, 0,	   1, "", ""},
	{ "fs_1046c",		0x18b86, 0x18bf2, 0,	   1, "", ""},
	{ "fs_10472",		0x18bf4, 0x18c90, 0,	   1, "sLsLsPsP", "sLsLsPsP"},
	{ "fs_10478",		0x18c92, 0x18d22, 0,	   1, "", ""},
	{ "fs_1047e_exp_xmit",	0x18d24, 0x18d60, 0,	   1, "sEsB", "sE"},
	{ "fs_10484",		0x18d62, 0x18df6, 0,	   1, "sPsPsP", "sPsPsP"},
	{ "fs_1048a",		0x18df8, 0x18e40, 0,	   1, "", ""},
	{ "fs_10490",		0x18e42, 0x18eea, 0,	   1, "", ""},
	{ "fs_10496_exp_close",	0x18eec, 0x18ff0, 0,	   1, "", ""},
	{ "fs_1049c",		0x18ff2, 0x190a2, 0,	   1, "", ""},
	{ "fs_104a2",		0x190a4, 0x190f8, 0,	   1, "", ""},
	{ "fs_104a8",		0x190fa, 0x19168, 0,	   1, "", ""},
	{ "fs_104ae",		0x1916a, 0x191d4, 0,	   1, "", ""},
	{ "fs_104b4",		0x191d6, 0x1927c, 0,	   1, "", ""},
	{ "fs_104ba",		0x1927e, 0x194f4, 0,	   1, "sEsBsBsW", "sEsBsBsW"},
	{ "fs_104c0",		0x194f6, 0x195fc, 0,	   1, "", ""},
	{ "fs_10568",		0x1a118, 0x1a232, 0,	   1, "x2", ""},
	{ "fs_10592",		0x1a96a, 0x1a9ba, 0,	   1, "sLsW", "sLsW"},
	{ "fs_10610",		0x1afd0, 0x1b01e, 0x1b020, 0, "", "sB"},

	{ "novram_0",		0x21cda, 0,	  0,	   1, "sB", ""},
	{ "novram_0",		0x21e48, 0,	  0,	   1, "A1", ""},

	{ NULL, 0, 0, 0, 0, NULL, NULL },
};

void v_matchproto_(cli_func_f)
cli_ioc_syscall(struct cli *cli)
{
	struct syscall *sc;

	if (cli->help) {
		cli_io_help(cli, "ioc syscall", 0, 1);
		return;
	}

	cli->ac--;
	cli->av++;

	for (sc = syscalls; sc->name != NULL; sc++) {
		mem_peg_register(sc->lo, sc->lo+ 2, sc_peg, sc);
		if (sc->hi)
			mem_peg_register(sc->hi, sc->hi + 2, sc_peg, sc);
		if (sc->no_trace)
			mem_peg_set(sc->lo, sc->no_trace, PEG_NOTRACE);
	}
	while (cli->ac && !cli->status) {
		cli_unknown(cli);
		break;
	}
}

