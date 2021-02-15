
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
dump_string(struct vsb *vsb, unsigned adr)
{
	unsigned u, v, c;

	u = vbe16dec(ram_space + adr);
	VSB_printf(vsb, "len=0x%x '", u);
	for (v = 0; v < u; v++) {
		c = ram_space[adr + 2 + v];
		if (c < 0x20 || 0x7e < c)
			VSB_cat(vsb, "␥");
		else
			VSB_putc(vsb, c);
	}
	VSB_cat(vsb, "'");
}

static void
dump_1020a(struct vsb *vsb, unsigned a7)
{
	unsigned u;

	u = vbe32dec(ram_space + a7 + 4);
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
dump_102c4(struct vsb *vsb, unsigned a7)
{
	unsigned l, u, v, c;

	l = vbe16dec(ram_space + a7 + 4);
	VSB_printf(vsb, " 0x%04x", l);
	VSB_printf(vsb, " 0x%04x", vbe16dec(ram_space + a7 + 6));
	u = vbe32dec(ram_space + a7 + 8);
	VSB_printf(vsb, " 0x%08x '", u);
	for (v = 0; v < l; v++) {
		c = ram_space[u + v];
		if (c < 0x20 || 0x7e < c)
			VSB_cat(vsb, "␥");
		else
			VSB_putc(vsb, c);
	}
	VSB_printf(vsb, "'\n");
}

static void
dump_102d0(struct vsb *vsb, unsigned a7)
{
	unsigned u, v;

	u = vbe32dec(ram_space + a7 + 4);
	VSB_printf(vsb, " 0x%08x", u);
	v = vbe32dec(ram_space + a7 + 8);
	VSB_printf(vsb, " 0x%08x ", v);
	dump_string(vsb, v);
	VSB_printf(vsb, " ");
	dump_string(vsb, u);
	VSB_printf(vsb, "\n");
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
	case 0x102c4: dump_102c4(syscall_vsb, a7); break;
	case 0x102d0: dump_102d0(syscall_vsb, a7); break;
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
