
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
	struct sim *cs = r1000sim;

	if (cs->fd_trace < 0 || !(cs->do_trace & lvl))
		return;
	if (syscall_vsb == NULL)
		syscall_vsb = VSB_new_auto();
	AN(syscall_vsb);
	VSB_clear(syscall_vsb);
	VSB_printf(syscall_vsb, "%12jd Registers\n", cs->simclock);
	ioc_dump_regs(syscall_vsb);
	AZ(VSB_finish(syscall_vsb));
	VSB_tofile(syscall_vsb, cs->fd_trace);
}

static void
dump_text(struct vsb *vsb, unsigned adr, unsigned len)
{
	unsigned u, c;

	VSB_cat(vsb, "'");
	for (u = adr; u < adr + len; u++) {
		c = m68k_debug_read_memory_8(u);
		if (c < 0x20 || 0x7e < c)
			VSB_cat(vsb, "␥");
		else
			VSB_putc(vsb, c);
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
dump_1020a(struct vsb *vsb, unsigned a7)
{
	unsigned u;

	VSB_printf(vsb, " 0x%08x", vbe32dec(ram_space + a7 + 4));
	VSB_printf(vsb, " 0x%04x ", vbe16dec(ram_space + a7 + 8));
	u = vbe32dec(ram_space + a7 + 4);
	dump_string(vsb, u);
	VSB_cat(vsb, "\n");

	ioc_dump_regs(syscall_vsb);
	hexdump(syscall_vsb, ram_space + a7, 0x80, a7);
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

static void
dump_103d8(struct vsb *vsb, unsigned a7)
{
	(void)vsb;
	unsigned u;
	u = vbe32dec(ram_space + a7 + 4);
	VSB_printf(vsb, "0x%08x ", u);
	dump_string(vsb, u);
	VSB_cat(vsb, "\n");
}

void
ioc_trace_syscall(unsigned pc)
{
	unsigned a7, u;
	struct sim *cs = r1000sim;

	if (cs->fd_trace < 0 || !(cs->do_trace & TRACE_SYSCALL))
		return;
	if (syscall_vsb == NULL)
		syscall_vsb = VSB_new_auto();
	AN(syscall_vsb);
	VSB_clear(syscall_vsb);
	VSB_printf(syscall_vsb, "%12jd Syscall 0x%x ", cs->simclock, pc);

	a7 =  m68k_get_reg(NULL, M68K_REG_A7);
	switch (pc) {
	case 0x10204: dump_10204(syscall_vsb, a7); break;
	case 0x1020a: dump_1020a(syscall_vsb, a7); break;
	case 0x103d8: dump_103d8(syscall_vsb, a7); break;
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
	VSB_tofile(syscall_vsb, cs->fd_trace);
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
	struct sim *cs = r1000sim;
	const char *which;
	unsigned a7, sp, u, v;

	(void)val;
	(void)what;
	(void)width;
	(void)peg;
	if (ioc_fc != 2)
		return;

	if (syscall_vsb == NULL)
		syscall_vsb = VSB_new_auto();
	AN(syscall_vsb);
	VSB_clear(syscall_vsb);
	VSB_printf(syscall_vsb, "%12jd ", cs->simclock);

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
			// 16 bit long word on stack
			u = m68k_debug_read_memory_16(sp);
			VSB_printf(syscall_vsb, "\t\tWord: 0x%x\n", u);
			sp += 2;
			continue;
		}
		if (which[0] == 's' && which[1] == 'P') {
			// 32 bit pointer on stack
			u = m68k_debug_read_memory_32(sp);
			VSB_printf(syscall_vsb, "\t\tPointer: 0x%x\n", u);
			sp += 4;
			continue;
		}
		if (which[0] == 's' && which[1] == 'Q') {
			// 32 bit pointer to pointer on stack
			u = m68k_debug_read_memory_32(sp);
			VSB_printf(syscall_vsb, "\t\tPointer: 0x%x", u);
			u = m68k_debug_read_memory_32(u);
			VSB_printf(syscall_vsb, " *-> 0x%x\n", u);
			sp += 4;
			continue;
		}
		if (which[0] == 's' && which[1] == 'S') {
			// 32 bit pointer to string on stack
			u = m68k_debug_read_memory_32(sp);
			VSB_printf(syscall_vsb, "\t\tString: 0x%x", u);
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
		WRONG();
	}
	if (sc->dump) {
		VSB_printf(syscall_vsb, "PC = 0x%x\n", adr);
		ioc_dump_regs(syscall_vsb);
		hexdump(syscall_vsb, ram_space + a7, 0x80, a7);
	}
	AZ(VSB_finish(syscall_vsb));
	VSB_tofile(syscall_vsb, cs->fd_trace);
}

static struct syscall syscalls[] = {
	{ "FREE",	0x000108fa, 0x000109fc, 1, 0, "sLsQ", "" },
	{ "STRCAT",	0x00010f2c, 0x00010fc8, 1, 0, "sSsS", "ilsS"},
	{ "MKSTR",	0x00010da4, 0x00010e60, 1, 0, "sWsWx0", "iwiwilsS"},
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
		mem_peg_register(sc->hi, sc->hi + 2, sc_peg, sc);
		if (sc->no_trace)
			mem_peg_set(sc->lo, sc->hi + 2, PEG_NOTRACE);
	}
	while (cli->ac && !cli->status) {
		cli_unknown(cli);
		break;
	}
}

