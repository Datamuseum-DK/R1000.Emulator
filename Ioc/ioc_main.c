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
#include "vend.h"

#include "memspace.h"

#define IOC_CPU_TYPE	M68K_CPU_TYPE_68020

/* Prototypes */
// static void exit_error(char* fmt, ...);

static uint8_t resha_eeprom[32768];

unsigned ioc_fc;

unsigned int ioc_pc;

static uintmax_t go_until_increment = 0;
static uintmax_t go_until = 1000000000;

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

void v_matchproto_(cli_func_f)
cli_ioc_main(struct cli *cli)
{

	if (cli->help) {
		cli_io_help(cli, "IOC main", 0, 1);
		return;
	}

	cli->ac--;
	cli->av++;

	dump_ram();

	while (cli->ac && !cli->status) {
		if (cli->ac >= 2 && !strcmp(cli->av[0], "go_until_increment")) {
			go_until_increment = strtoumax(cli->av[1], NULL, 0);
			cli->ac -= 2;
			cli->av += 2;
			continue;
		}
		cli_unknown(cli);
		break;
	}
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
GENERIC_POST_WRITE(f000)
GENERIC_POST_WRITE(f200)
GENERIC_POST_WRITE(f300)
GENERIC_POST_WRITE(io_sreg4)
GENERIC_POST_WRITE(f500)
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

void v_matchproto_(mem_post_write)
resha_page_post_write(int debug, uint8_t *space, unsigned width, unsigned adr)
{
	(void)debug;
	(void)adr;
	// See 0x80000fa8-80000fb4
	trace(TRACE_IO, "RESHA_PAGE W [%x] <- %x/%d\n", adr, space[adr], width);
	space[1] = space[0];
}

void v_matchproto_(mem_pre_read)
resha_eeprom_pre_read(int debug, uint8_t *space, unsigned width, unsigned adr)
{
	unsigned u;

	(void)debug;
	u = (resha_page_space[0] << 8) | adr;
	u &= 0x7fff;
	memcpy(space + adr, resha_eeprom + u, width);
	trace(TRACE_IO, "RESHA_PAGE R [%x] -> %x/%d\n", adr, space[adr], width);
}

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
	uint8_t *peg = NULL;
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
			trace(TRACE_68K, "E … × %x\n", repeats);
		repeats = 0;
		trace(
		    TRACE_68K,
		    "E %08x %04x: %02x %-20s: %s\n",
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

	if (0 && 0x00018d24 <= pc && pc <= 0x00018d60)
		ioc_dump_registers(TRACE_68K);
	if (r1000sim->do_trace)
		cpu_trace(pc);
	if (0x10000 <= pc && pc <= 0x1061c)
		ioc_trace_syscall(pc);

	if (pc == 0x80000088) {
		// hit self-test fail, stop tracing
		ioc_dump_registers(TRACE_68K);
		r1000sim->do_trace = 0;
	}
	if (pc == 0x100087ce) {
		ioc_dump_registers(TRACE_68K);
		r1000sim->do_trace = 0;
		printf("DIAGTX breakpoint\n");
		crash();
	}
	if (pc == 0x00071286 || pc == 0x7056e || pc == 0x766a2) {
		ioc_dump_registers(TRACE_68K);
		r1000sim->do_trace = 0;
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
		r1000sim->do_trace = 0;
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

static void
insert_jump(unsigned int from, unsigned int to)
{

	from &= 0x7fffffff;
	ioc_eeprom_space[from] = 0x4e;
	ioc_eeprom_space[from+1L] = 0xf9;
	vbe32enc(ioc_eeprom_space + from + 2, to);
}

void
ioc_keep_going(void)
{
	if (go_until_increment) {
		go_until = r1000sim->simclock + go_until_increment;
		trace(TRACE_IO, "GO UNTIL %ju\n", go_until);
	}
}

/* The main loop */
void *
main_ioc(void *priv)
{
	FILE* fhandle;
	unsigned last_irq_level = 0;

	(void)priv;
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	fhandle = fopen("IOC_EEPROM.bin", "rb");
	AN(fhandle);
	assert(fread(ioc_eeprom_space + 0x0000, 1, 8192, fhandle) == 8192);
	assert(fread(ioc_eeprom_space + 0x4000, 1, 8192, fhandle) == 8192);
	assert(fread(ioc_eeprom_space + 0x2000, 1, 8192, fhandle) == 8192);
	assert(fread(ioc_eeprom_space + 0x6000, 1, 8192, fhandle) == 8192);
	AZ(fclose(fhandle));
	memcpy(ram_space, ioc_eeprom_space, 8);

	Ioc_HotFix_Ioc(ioc_eeprom_space);

	fhandle = fopen("RESHA_EEPROM.bin", "rb");
	AN(fhandle);
	assert(fread(resha_eeprom + 0x0000, 1, 32768, fhandle) == 32768);
	AZ(fclose(fhandle));

	Ioc_HotFix_Resha(resha_eeprom);

	insert_jump(0x800001e4, 0x8000021a); // EEPROM CHECKSUM
	insert_jump(0x800003a4, 0x80000546); // 512k RAM Test
	insert_jump(0x80000568, 0x800007d0); // Parity
	insert_jump(0x800007f4, 0x800009b2); // I/O Bus control
	insert_jump(0x800009da, 0x80000a4a); // I/O Bus map parity
	insert_jump(0x80000a74, 0x80000b8a); // I/O bus transactions

	//insert_jump(0x80000ba2, 0x80000bf2); // PIT  (=> DUART)
	//insert_jump(0x80000c1a, 0x80000d20); // Modem DUART channel
	//insert_jump(0x80000d4e, 0x80000dd6); // Diagnostic DUART channel
	//insert_jump(0x80000dfc, 0x80000ec4); // Clock / Calendar

	// insert_jump(0x80000fa0, 0x80000fda); // RESHA EEProm Interface ...

	insert_jump(0x80001170, 0x8000117c); // RESHA VEM sub-tests
	insert_jump(0x8000117c, 0x80001188); // RESHA LANCE sub-tests
	//insert_jump(0x80001188, 0x80001194); // RESHA DISK SUB-TESTs
	insert_jump(0x80001194, 0x800011a0); // RESHA TAPE SUB-TESTs

	//insert_jump(0x800011c0, 0x800014d0); // Local interrupts

	insert_jump(0x80001502, 0x800015ce); // Illegal reference protection
	insert_jump(0x800015f2, 0x8000166c); // I/O bus parity
	insert_jump(0x8000169c, 0x800016d8); // I/O bus spurious interrupts
	insert_jump(0x80001700, 0x80001746); // Temperature sensors
	insert_jump(0x80001774, 0x800017f8); // IOC diagnostic processor
	insert_jump(0x8000181c, 0x8000185c); // Power margining
	insert_jump(0x80001880, 0x8000197c); // Clock margining
	insert_jump(0x80001982, 0x80001992); // final check

	// Local interrupts test
	insert_jump(0x800011dc, 0x800011fc); // XXX: Where does vector 0x50 come from ?!
	insert_jump(0x8000127a, 0x80001298); // XXX: Where does vector 0x51 come from ?!
	insert_jump(0x80001358, 0x80001470); // XXX: Where does vector 0x52 come from ?!

	m68k_init();
	m68k_set_cpu_type(IOC_CPU_TYPE);
	m68k_pulse_reset();

	io_sreg8_space[3] = 0x7;
	while(1)
	{
		if (irq_level != last_irq_level) {
			last_irq_level = irq_level;
			m68k_set_irq(last_irq_level);
			trace(TRACE_68K, "IRQ level %x\n", last_irq_level);
			io_sreg8_space[3] &= ~7;
			io_sreg8_space[3] |= (~irq_level) & 7;
		}
		r1000sim->simclock += 100 * m68k_execute(1);
		callout_poll(r1000sim);
		if (go_until_increment && r1000sim->simclock > go_until) {
			printf("Ran clock out\n");
			exit(0);
		}
	}

	return NULL;
}

void
ioc_init(struct sim *cs)
{
	ioc_console_init(cs);
	ioc_duart_init(cs);
	ioc_scsi_d_init(cs);
	ioc_scsi_t_init(cs);
	ioc_rtc_init(cs);
}
