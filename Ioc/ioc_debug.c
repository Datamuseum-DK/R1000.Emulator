/*-
 * Copyright (c) 2021 Poul-Henning Kamp
 * All rights reserved.
 *
 * Author: Poul-Henning Kamp <phk@phk.freebsd.dk>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <stdio.h>

#include "r1000.h"
#include "m68k.h"
#include "ioc.h"
#include "vsb.h"
#include "memspace.h"

static struct vsb *syscall_vsb;

void
ioc_dump_registers(unsigned lvl)
{

	if (trace_fd < 0 || !(do_trace & lvl))
		return;
	if (syscall_vsb == NULL)
		syscall_vsb = VSB_new_auto();
	AN(syscall_vsb);
	VSB_clear(syscall_vsb);
	VSB_printf(syscall_vsb, "%12jd Registers\n", simclock);
	ioc_dump_cpu_regs(syscall_vsb);
	AZ(VSB_finish(syscall_vsb));
	VSB_tofile(syscall_vsb, trace_fd);
}

void
ioc_dump_cpu_regs(struct vsb *vsb)
{
	VSB_printf(vsb, "PC = 0x%x", m68k_get_reg(NULL, M68K_REG_PC));
	VSB_printf(vsb, "  SR = 0x%x\n", m68k_get_reg(NULL, M68K_REG_SR));
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
ioc_dump_core(const char *fn)
{   
        FILE *f;

	AN(fn);
        f = fopen(fn, "w");
        assert (f != NULL);
        (void)fwrite(ram_space, sizeof(ram_space), 1, f);
        (void)fclose(f);
}

void v_matchproto_(cli_func_f)
cli_ioc_dump(struct cli *cli)
{
	const char *fn;

        if (cli->help) {
                cli_usage(cli, "\n\tDump IOC RAM to file\n");
                return;
        }
        cli->ac--;
        cli->av++;
        if (cli_n_m_args(cli, 0, 1, ""))
                return;
	if (cli->ac == 0)
		fn = "/tmp/_ioc.ram";
	else
		fn = cli->av[0];
	ioc_dump_core(fn);
	cli_printf(cli, "Core dumped to '%s'\n", fn);
}

