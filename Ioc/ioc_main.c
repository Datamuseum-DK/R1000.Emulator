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

#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "r1000.h"
#include "ioc.h"

#include "m68k.h"
#include "vsb.h"
#include "vend.h"

#include "memspace.h"

#define IOC_CPU_TYPE	M68K_CPU_TYPE_68020

static pthread_t ioc_cpu;

static uintmax_t ioc_maxins = 0;
uintmax_t ioc_nins = 0;
static uintmax_t ioc_cpu_quota = 0;
static unsigned ioc_cpu_running = 0;
static pthread_cond_t ioc_cpu_cond_state = PTHREAD_COND_INITIALIZER;
static pthread_cond_t ioc_cpu_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t ioc_cpu_mtx = PTHREAD_MUTEX_INITIALIZER;

unsigned ioc_fc;

unsigned int ioc_pc;

/**********************************************************************
 */

static void
ioc_stop_cpu(void)
{
	AZ(pthread_mutex_lock(&ioc_cpu_mtx));
	ioc_cpu_quota = 0;
	while (ioc_cpu_running)
		AZ(pthread_cond_wait(&ioc_cpu_cond_state, &ioc_cpu_mtx));
	AZ(pthread_mutex_unlock(&ioc_cpu_mtx));
}

static void
ioc_wait_cpu_stopped(void)
{
	AZ(pthread_mutex_lock(&ioc_cpu_mtx));
	while (ioc_cpu_quota || ioc_cpu_running)
		AZ(pthread_cond_wait(&ioc_cpu_cond_state, &ioc_cpu_mtx));
	AZ(pthread_mutex_unlock(&ioc_cpu_mtx));
}

static void
ioc_start_cpu(uintmax_t quota)
{
	if (!quota)
		quota = 1ULL << 60;

	AZ(pthread_mutex_lock(&ioc_cpu_mtx));
	AZ(ioc_cpu_running);
	ioc_cpu_quota = quota;
	AZ(pthread_cond_broadcast(&ioc_cpu_cond));
	AZ(pthread_mutex_unlock(&ioc_cpu_mtx));
}

void v_matchproto_(cli_func_f)
cli_ioc_reset(struct cli *cli)
{
	if (cli->help) {
		cli_usage(cli, "\n\tReset IOC CPU\n");
		return;
	}
	cli->ac--;
	cli->av++;
	if (cli_n_m_args(cli, 0, 1, "[-stop]"))
		return;
	ioc_stop_cpu();
	memcpy(ram_space, ioc_eeprom_space, 8);
	m68k_pulse_reset();
	if (cli->ac == 0)
		ioc_start_cpu(0);
}

void v_matchproto_(cli_func_f)
cli_ioc_state(struct cli *cli)
{
	struct vsb *vsb;

	vsb = VSB_new_auto();
	AN(vsb);
	AZ(pthread_mutex_lock(&ioc_cpu_mtx));
	if (ioc_cpu_running)
		VSB_printf(vsb, "IOC CPU is running\n");
	else
		VSB_printf(vsb, "IOC CPU is stopped\n");
	ioc_dump_cpu_regs(vsb);
	AZ(pthread_mutex_unlock(&ioc_cpu_mtx));
	AZ(VSB_finish(vsb));
	cli_printf(cli, "%s", VSB_data(vsb));
	VSB_destroy(&vsb);
}

void v_matchproto_(cli_func_f)
cli_ioc_step(struct cli *cli)
{
	uintmax_t uj = 1;

	if (cli->help) {
		cli_usage(cli, " [count]\n\tSingle step IOC CPU\n");
		return;
	}
	cli->ac--;
	cli->av++;
	if (cli_n_m_args(cli, 0, 1, "[count]"))
		return;
	if (cli->ac == 1)
		uj = strtoumax(*cli->av, NULL, 0);
	if (ioc_cpu_running) {
		cli_error(cli, "IOC CPU is running\n");
		return;
	}
	ioc_start_cpu(uj);
	ioc_wait_cpu_stopped();
	cli_ioc_state(cli);
}

void v_matchproto_(cli_func_f)
cli_ioc_stop(struct cli *cli)
{
	if (cli->help) {
		cli_usage(cli, "\n\tStop IOC CPU\n");
		return;
	}
	cli->ac--;
	cli->av++;
	if (cli_n_m_args(cli, 0, 0, ""))
		return;
	ioc_stop_cpu();
	ioc_wait_cpu_stopped();
	cli_ioc_state(cli);
}

void v_matchproto_(cli_func_f)
cli_ioc_start(struct cli *cli)
{
	if (cli->help) {
		cli_usage(cli, "\n\tStart IOC CPU\n");
		return;
	}
	cli->ac--;
	cli->av++;
	if (cli_n_m_args(cli, 0, 0, ""))
		return;
	ioc_start_cpu(0);
}

void v_matchproto_(cli_func_f)
cli_ioc_maxins(struct cli *cli)
{

	if (cli->help) {
		cli_usage(cli,
		    " [count]\n\tTerminate after `count` instructions\n");
		return;
	}
	cli->ac--;
	cli->av++;
	if (cli_n_m_args(cli, 1, 1, "[count]"))
		return;
	ioc_maxins = strtoumax(*cli->av, NULL, 0);
}

/**********************************************************************
 */
static void
dump_ram(void)
{
	int fd;

	fd = open("/tmp/_.ram", O_WRONLY|O_CREAT|O_TRUNC, 0644);
	assert (fd>0);
	(void)write(fd, ram_space, sizeof(ram_space));
	(void)close(fd);
}

static void
crash(void)
{
	ioc_dump_registers(TRACE_68K);
	dump_ram();
	exit(2);
}

/**********************************************************************
 */

#define GENERIC_POST_WRITE(name)							\
	void v_matchproto_(mem_post_write)						\
	name##_post_write(int debug, uint8_t *space, unsigned width, unsigned adr)	\
	{										\
		unsigned v;								\
		if (debug) return;							\
		if (width == 1)								\
			v = space[adr];							\
		else if (width == 2)							\
			v = vbe16dec(space + adr);					\
		else									\
			v = vbe32dec(space + adr);					\
		trace(TRACE_IO, "PW " #name " W(%d) 0x%x <- 0x%x\n", width, adr, v);	\
	}

GENERIC_POST_WRITE(fb000)
GENERIC_POST_WRITE(fifo_response_latch)
GENERIC_POST_WRITE(f200)
GENERIC_POST_WRITE(f300)
GENERIC_POST_WRITE(io_sreg4)
GENERIC_POST_WRITE(f500)
GENERIC_POST_WRITE(fifo_response)
GENERIC_POST_WRITE(fifo_request)
GENERIC_POST_WRITE(io_sreg8)
GENERIC_POST_WRITE(f900)
GENERIC_POST_WRITE(fc00)
GENERIC_POST_WRITE(fd00)
GENERIC_POST_WRITE(fe00)


/**********************************************************************
 * I/O Address mapping
 *
 * The 0x600 comes from pin 6 & 10 pull-ups on L41 IOCp33
 */

void
dma_write(unsigned segment, unsigned address, void *src, unsigned len)
{
	unsigned int u, v;

	u = 0x600;
	u |= (segment & 0x7) << 6;
	u |= address >> 10;
	v = vbe32dec(io_map_space + u * 4L);
	trace(TRACE_IO, "MAP_DMA_W %08x:%08x: %08x -> %08x [%08x]\n", segment, address, u, v, len);
	memcpy(ram_space+v, src, len);
}

void
dma_read(unsigned segment, unsigned address, void *src, unsigned len)
{
	unsigned int u, v;

	u = 0x600;
	u |= address >> 10;
	u |= (segment & 0x7) << 6;
	v = vbe32dec(io_map_space + u * 4L);
	trace(TRACE_IO, "MAP_DMA_R %08x:%08x: %08x -> %08x [%08x]\n",
	    segment, address, u, v, len);
	memcpy(src, ram_space+v, len);
}

/**********************************************************************/

void v_matchproto_(mem_pre_read)
io_sreg8_pre_read(int debug, uint8_t *space, unsigned width, unsigned adr)
{

	(void)debug;
	(void)width;
	(void)adr;
	space[3] |= 0x20;
}

/**********************************************************************/

void v_matchproto_(mem_pre_read)
irq_vector_pre_read(int debug, uint8_t *space, unsigned width, unsigned adr)
{
	unsigned v;

	if (debug) return;
	if (ioc_fc == 7) {
		v = irq_getvector(0);
		if (width == 1)
			space[adr] = v;
		else if (width == 2)
			vbe16enc(space + adr, v);
		else
			vbe32enc(space + adr, v);
		trace(TRACE_68K, "IRQ_VECTOR %x/%x (%x))\n", v, width, adr);
	}
}

unsigned int
m68k_read_disassembler_16(unsigned int address)
{
	return m68k_debug_read_memory_16(address);
}

unsigned int
m68k_read_disassembler_32(unsigned int address)
{
	return m68k_debug_read_memory_32(address);
}


/* Disassembler */
static void
make_hex(char* buff, unsigned int pc, unsigned int length)
{
	char* ptr = buff;

	for(;length>0;length -= 2)
	{
		sprintf(ptr, "%04x", m68k_read_disassembler_16(pc));
		pc += 2;
		ptr += 4;
		if(length > 2)
			*ptr++ = ' ';
	}
}

static void
cpu_trace(unsigned int pc)
{

	char buff[100];
	char buff2[100];
	uint8_t *peg;
	unsigned int instr_size;
	static unsigned int last_pc = 0;
	static unsigned repeats = 0;

	peg = mem_find_peg(pc);
	AN(peg);
	if (*peg & PEG_NOTRACE)
		return;
	instr_size = m68k_disassemble(buff, pc, IOC_CPU_TYPE);
	make_hex(buff2, pc, instr_size);
	if (pc != last_pc) {
		if (repeats)
			Trace(trace_ioc_instructions, "E … × %x", repeats);
		repeats = 0;
		Trace(
		    trace_ioc_instructions,
		    "E %08x %04x: %02x %-20s: %s",
		    pc,
		    m68k_get_reg(NULL, M68K_REG_SR),
		    *peg,
		    buff2,
		    buff
		);
		last_pc = pc;
	} else {
		repeats++;
	}
}

void
cpu_instr_callback(unsigned int pc)
{
	unsigned int a6;

	ioc_pc = pc;

#if 0
	if (0 && 0x00018ec8 <= pc && pc <= 0x00018eea)
		ioc_dump_registers(TRACE_68K);
#endif
	if (trace_ioc_instructions)
		cpu_trace(pc);

	if (pc == 0x80000088) {
		// hit self-test fail, stop tracing
		ioc_dump_registers(TRACE_68K);
		do_trace = 0;
	}
	if (pc == 0x100087ce) {
		ioc_dump_registers(TRACE_68K);
		do_trace = 0;
		printf("DIAGTX breakpoint\n");
		crash();
	}
	if (pc == 0x00071286 || pc == 0x7056e || pc == 0x766a2) {
		ioc_dump_registers(TRACE_68K);
		do_trace = 0;
		printf("RESHA test failed\n");
		crash();
	}
	if (pc == 0x800000b4) {
		a6 =  m68k_get_reg(NULL, M68K_REG_A6);
		printf("Self test at 0x%x failed\n", a6);
		crash();
	}
	if (pc == 0x0000a090) {
		ioc_dump_registers(TRACE_68K);
		// hit trap, stop tracing
		do_trace = 0;
	}
	if (pc == 0x80004d08) {
		printf("Hit debugger\n");
		crash();
	}
	if (pc == 0x000741e6) {
		printf("Resha jumps to bootloader\n");
		Ioc_HotFix_Bootloader(ram_space);
	}
	if (pc == 0x000540f6) {
		printf("Bootloader jumps to kernel\n");
		Ioc_HotFix_Kernel(ram_space);
	}
	if (pc == 0x0007678a) {
		printf("Resha tape loader jumps to kernel\n");
		Ioc_HotFix_Kernel(ram_space);
	}
}

/* The main loop */
void *
main_ioc(void *priv)
{
	unsigned last_irq_level = 0;
	int i;
	nanosec ns;

	(void)priv;

	io_sreg8_space[3] = 0x7;

	AZ(pthread_mutex_lock(&ioc_cpu_mtx));
	while(1)
	{
		while (!ioc_cpu_quota) {
			if (ioc_cpu_running)
				AZ(pthread_cond_broadcast(&ioc_cpu_cond_state));
			ioc_cpu_running = 0;
			AZ(pthread_cond_wait(&ioc_cpu_cond, &ioc_cpu_mtx));
		}
		ioc_cpu_running = 1;
		AZ(pthread_mutex_unlock(&ioc_cpu_mtx));

		if (irq_level != last_irq_level) {
			last_irq_level = irq_level;
			m68k_set_irq(last_irq_level);
			Trace(trace_ioc_interrupt,
			    "IRQ level 0x%x", last_irq_level);
			io_sreg8_space[3] &= ~7;
			io_sreg8_space[3] |= (~irq_level) & 7;
		}
		i = m68k_execute(1);
		simclock += 100ULL * i;
		ioc_nins++;
		if (ioc_maxins && ioc_nins > ioc_maxins) {
			printf("maxins reached, exiting\n");
			exit(4);
		}
		ns = callout_poll();
		if (i == 1) {
			ns -= simclock;
			usleep(1 + (ns / 1000));
			simclock += ns;
		}
		AZ(pthread_mutex_lock(&ioc_cpu_mtx));
		if (ioc_cpu_quota)
			ioc_cpu_quota--;
	}
	return(NULL);
}

void
ioc_init(void)
{
	m68k_init();
	m68k_set_cpu_type(IOC_CPU_TYPE);

	ioc_load_eeproms();

	ioc_console_init();
	ioc_duart_init();
	ioc_scsi_d_init();
	ioc_scsi_t_init();
	ioc_rtc_init();
	AZ(pthread_create(&ioc_cpu, NULL, main_ioc, NULL));
}
