#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "r1000.h"
#include "ioc.h"
#include "m68k.h"
#include "vend.h"

#define IOC_CPU_TYPE	M68K_CPU_TYPE_68020

/* Prototypes */
static void exit_error(char* fmt, ...);

static uint8_t ioc_eeprom[32768];
static uint8_t resha_eeprom[32768];
static uint8_t ram[1<<19];
static uint8_t map_dma_in[1<<13];

unsigned ioc_fc;

unsigned int ioc_pc;

static void
dump_ram(void)
{
	int fd;

	fd = open("/tmp/_.ram", O_WRONLY|O_CREAT|O_TRUNC, 0644);
	assert (fd>0);
	(void)write(fd, ram, sizeof(ram));
	(void)close(fd);
}

static void
dump_registers(void)
{
	trace(TRACE_68K, "D0 = %08x  D4 = %08x   A0 = %08x  A4 = %08x\n",
	    m68k_get_reg(NULL, M68K_REG_D0), m68k_get_reg(NULL, M68K_REG_D4),
	    m68k_get_reg(NULL, M68K_REG_A0), m68k_get_reg(NULL, M68K_REG_A4)
	);

	trace(TRACE_68K, "D1 = %08x  D5 = %08x   A1 = %08x  A5 = %08x\n",
	    m68k_get_reg(NULL, M68K_REG_D1), m68k_get_reg(NULL, M68K_REG_D5),
	    m68k_get_reg(NULL, M68K_REG_A1), m68k_get_reg(NULL, M68K_REG_A5)
	);

	trace(TRACE_68K, "D2 = %08x  D6 = %08x   A2 = %08x  A6 = %08x\n",
	    m68k_get_reg(NULL, M68K_REG_D2), m68k_get_reg(NULL, M68K_REG_D6),
	    m68k_get_reg(NULL, M68K_REG_A2), m68k_get_reg(NULL, M68K_REG_A6)
	);

	trace(TRACE_68K, "D3 = %08x  D7 = %08x   A3 = %08x  A7 = %08x\n",
	    m68k_get_reg(NULL, M68K_REG_D3), m68k_get_reg(NULL, M68K_REG_D7),
	    m68k_get_reg(NULL, M68K_REG_A3), m68k_get_reg(NULL, M68K_REG_A7)
	);

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
		cli_unknown(cli);
		break;
	}
}

/**********************************************************************/

void
dma_write(unsigned segment, unsigned address, void *src, unsigned len)
{
	unsigned int u, v;

	u = segment << 9;
	u |= (address >> 10) & 0x1ff;
	v = vbe32dec(map_dma_in + u * 4L);
	trace(TRACE_IO, "DMAMAP %08x: %08x -> %08x\n", address, u, v);
	memcpy(ram+v, src, len);

	// Patch delay routines faster
	if (ram[0x66d3] == 0x23)
		ram[0x66d3] = 0;
	if (ram[0x5d1a] == 0x05)
		ram[0x5d1a] = 1;
	if (ram[0x9af9] == 0x07)
		ram[0x9af9] = 0;
	if (ram[0x9afa] == 0xff)
		ram[0x9afa] = 0;
}

static unsigned int v_matchproto_(iofunc_f)
io_map_dma_in(
    const char *op,
    unsigned int address,
    memfunc_f *func,
    unsigned int value
)
{

	IO_TRACE_WRITE(2, "MAP_DMA_IN");
	value = func(op, map_dma_in, address & 0x1fff, value);
	IO_TRACE_READ(2, "MAP_DMA_IN");
	return (value);
}

static unsigned int v_matchproto_(iofunc_f)
io_map_dma_out(
    const char *op,
    unsigned int address,
    memfunc_f *func,
    unsigned int value
)
{

	(void)func;
	IO_TRACE_WRITE(2, "MAP_DMA_OUT");
	// value = func(op, map_dma_out, (address>>8) & 0x1fff, value);
	IO_TRACE_READ(2, "MAP_DMA_OUT");
	return (value);
}

/**********************************************************************/

/* Exit with an error message.  Use printf syntax. */
static void
exit_error(char* fmt, ...)
{
	static int guard_val = 0;
	char buff[100];
	unsigned int pc;
	va_list args;

	if(guard_val)
		return;
	else
		guard_val = 1;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
	pc = m68k_get_reg(NULL, M68K_REG_PPC);
	m68k_disassemble(buff, pc, IOC_CPU_TYPE);
	fprintf(stderr, "At %04x: %s\n", pc, buff);

	exit(EXIT_FAILURE);
}

static unsigned int  v_matchproto_(memfunc_f)
rdbyte(const char *op, uint8_t *space, unsigned int address, unsigned int data)
{
	(void)op;
	(void)data;
	return space[address];
}

static unsigned int  v_matchproto_(memfunc_f)
rdword(const char *op, uint8_t *space, unsigned int address, unsigned int data)
{
	(void)op;
	(void)data;
	return (vbe16dec(space + address));
}

static unsigned int  v_matchproto_(memfunc_f)
rdlong(const char *op, uint8_t *space, unsigned int address, unsigned int data)
{
	(void)op;
	(void)data;
	return (vbe32dec(space + address));
}

static unsigned int  v_matchproto_(memfunc_f)
wrbyte(const char *op, uint8_t *space, unsigned int address, unsigned int data)
{
	(void)op;
	space[address] = data;
	return (0);
}

static unsigned int  v_matchproto_(memfunc_f)
wrword(const char *op, uint8_t *space, unsigned int address, unsigned int data)
{
	(void)op;
	vbe16enc(space + address, data);
	return (0);
}

static unsigned int  v_matchproto_(memfunc_f)
wrlong(const char *op, uint8_t *space, unsigned int address, unsigned int data)
{
	(void)op;
	vbe32enc(space + address, data);
	return (0);
}

/**********************************************************************/

static uint8_t reg_400[4];
static uint8_t reg_800[4];

static unsigned int
io_registers(const char *op, unsigned int address, memfunc_f *func, unsigned int value)
{

	IO_TRACE_WRITE(TRACE_IO, "STATUS_REGISTER");
	if (address == 0xfffff400) {
		value = func(op, reg_400, address & 3, value);
	} else if (0xfffff800 == (address & ~3)) {
		reg_800[3] |= 0x20;
		value = func(op, reg_800, address & 3, value);
		reg_800[3] |= 0x20;
	} else {
		exit_error("Attempted io_reg memory at address %08x", address & ~3);
	}

	IO_TRACE_READ(TRACE_IO, "STATUS_REGISTER");
	return (value);
}


/**********************************************************************/

static unsigned int v_matchproto_(iofunc_f)
io_resha_eeprom(
    const char *op,
    unsigned int address,
    memfunc_f *func,
    unsigned int value
)
{
	unsigned int u;
	static uint8_t eeprom_page[4];

	// IO_TRACE_WRITE(2, "RESHA_EEPROM");
	if (address == 0x9303e00a) {
		value = func(op, eeprom_page, address&3, value);
		if (op[0] == 'W') {
			// See 0x80000fa8 and 0x80000fae
			eeprom_page[3] = eeprom_page[2];
		}
	}
	if (0x9303e300 == (address & ~0xff)) {
		u = (eeprom_page[2]<<8) | (address & 0xff);
		u &= 0x7fff;
		if (op[0] == 'R')
			value = resha_eeprom[u];
	}
	// IO_TRACE_READ(2, "RESHA_EEPROM");
	return (value);
}

/**********************************************************************/

static unsigned int v_matchproto_(iofunc_f)
io_resha_wcard(
    const char *op,
    unsigned int address,
    memfunc_f *func,
    unsigned int value
)
{
	static uint8_t bla[1<<16];

	IO_TRACE_WRITE(2, "RESHA_WCARD");
	value = func(op, bla, address & 0xffff, value);
	IO_TRACE_READ(2, "RESHA_WCARD");
	return (value);
}

/**********************************************************************/

static unsigned int
mem(const char *op, unsigned int address, memfunc_f *func, unsigned int value)
{
	unsigned retval;
// trace(TRACE_68K, "MEM %s %x\n", op, address);
	if (ioc_fc == 7 && op[0] != 'D') {
		retval = irq_getvector(0);
		trace(TRACE_68K, "IRQ_VECTOR %x (%08x %s %08x %x)\n",
		    retval, ioc_pc, op, address, value);
		assert ((address & ~0xf) == 0xfffffff0);
		return (retval);
	}
	if (address < 0x8000 && r1000sim->simclock < 1000)
		return func(op, ioc_eeprom, address & 0x7fffffff, value);
	if (address <= sizeof ram)
		return func(op, ram, address & 0x7fffffff, value);
	if (0x80000000 <= address && address <= 0x80007fff)
		return func(op, ioc_eeprom, address & 0x7fffffff, value);
	if (0x9303e00a == address) // resha eeprom page register
		return io_resha_eeprom(op, address, func, value);
	if (0x9303e300 == (address & ~0xff)) // resha eeprom page
		return io_resha_eeprom(op, address, func, value);

	if (0x9303e000 == (address & ~0x1ff)) // SCSI D CTL
		return io_scsi_ctl(op, address, func, value);
	if (0x9303e800 == (address & ~0x1f)) // SCSI D
		return io_scsi_d_reg(op, address, func, value);
	if (0x9303ec00 == (address & ~0x1f)) // SCSI T
		return io_scsi_t_reg(op, address, func, value);

	if (0x93030000 == (address & ~0xffff)) // SCSI A DMA
		return io_resha_wcard(op, address, func, value);
	if (0xa1000000 == (address & ~0xfffff))
		return io_map_dma_in(op, address, func, value);
	if (0xa2000000 == (address & 0xff000000))
		return io_map_dma_out(op, address, func, value);
	if (0xffff8000 == (address & ~0xfff))
		return io_rtc(op, address, func, value);
	if (0xffff9000 == (address & ~0xfff))
		return io_console_uart(op, address, func, value);
	if (0xffffa000 == (address & ~0xfff))
		return io_duart(op, address, func, value);
	if (0xffffb000 == (address & ~0xfff))
		return (0);		// EXT MODEM
	if (0xffffff03 == address)	// ?
		return (0);
	if (0xfffff200 == address)	// IO_FRONT_PANEL_LED_p27
		return (0);
	if (0xfffff300 == address)	// IO_SENREG_p25
		return (0);
	if (0xfffff400 == address)	// IO_DREG5_p24
		return io_registers(op, address, func, value);
	if (0xfffff500 == address)	// IO_FIFO_INIT_p68_p69
		return (0);
	if (0xfffff800 == (address & ~0xff))	// IO_READ_STATUS_p24
		return io_registers(op, address, func, value);
	if (0xfffff900 == address)	// IO_CLEAR_BERR_p24
		return (0);
	if (0xfffffc00 == address)	// IO_CONTROL_p28
		return (0);
	if (0xfffffd00 == address)	// IO_CLR_PFINT_p23
		return (0);
	if (0xfffffe00 == address)	// IO_CPU_CONTROL_PSU_MARGIN_BREG4_p23
		return (0);
	if (0xffffff00 == address)	// IO_READ_SENSE_p25
		return (0);

	exit_error("Attempted memory at address %08x", address);
	return (0);
}

/* Read data from RAM, ROM, or a device */
unsigned int
m68k_read_memory_8(unsigned int address)
{
	return mem("Rb", address, rdbyte, 0);
}

unsigned int
m68k_read_memory_16(unsigned int address)
{
	return mem("Rw", address, rdword, 0);
}

unsigned int
m68k_read_disassembler_16(unsigned int address)
{
	return mem("Dw", address, rdword, 0);
}

unsigned int
m68k_read_memory_32(unsigned int address)
{
	return mem("Rl", address, rdlong, 0);
}

unsigned int
m68k_read_disassembler_32(unsigned int address)
{
	return mem("Dl", address, rdlong, 0);
}

/* Write data to RAM or a device */
void
m68k_write_memory_8(unsigned int address, unsigned int value)
{
	(void)mem("Wb", address, wrbyte, value);
}

void
m68k_write_memory_16(unsigned int address, unsigned int value)
{
	(void)mem("Ww", address, wrword, value);
}

void
m68k_write_memory_32(unsigned int address, unsigned int value)
{
	(void)mem("Wl", address, wrlong, value);
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

void
cpu_instr_callback(unsigned int pc)
{

	char buff[100];
	char buff2[100];
	unsigned int instr_size, a6;
	static unsigned int last_pc = 0;
	static unsigned repeats = 0;

	ioc_pc = pc;
	if (r1000sim->do_trace) {
		instr_size = m68k_disassemble(buff, pc, IOC_CPU_TYPE);
		make_hex(buff2, pc, instr_size);
		if (pc != last_pc) {
			if (repeats)
				trace(TRACE_68K, "E … × %x\n", repeats);
			repeats = 0;
			trace(
			    TRACE_68K,
			    "E %08x: %-20s: %s\n",
			    pc,
			    buff2,
			    buff
			);
			last_pc = pc;
		} else {
			repeats++;
		}
	}
	if (pc == 0x80000088) {
		// hit self-test fail, stop tracing
		dump_registers();
		r1000sim->do_trace = 0;
	}
	if (pc == 0x00071286 || pc == 0x7056e) {
		dump_registers();
		r1000sim->do_trace = 0;
		printf("RESHA test failed\n");
		dump_ram();
		exit(2);
	}
	if (pc == 0x800000b4) {
		a6 =  m68k_get_reg(NULL, M68K_REG_A6);
		printf("Self test at 0x%x failed\n", a6);
		dump_registers();
		dump_ram();
		exit(2);
	}
	if (pc == 0x0000a090) {
		dump_registers();
		// hit trap, stop tracing
		r1000sim->do_trace = 0;
	}
	if (pc == 0x80004d08) {
		printf("Hit debugger\n");
		dump_ram();
		exit(2);
	}
}

static void
insert_jump(unsigned int from, unsigned int to)
{

	from &= 0x7fffffff;
	ioc_eeprom[from] = 0x4e;
	ioc_eeprom[from+1L] = 0xf9;
	vbe32enc(ioc_eeprom + from + 2, to);
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
	assert(fread(ioc_eeprom + 0x0000, 1, 8192, fhandle) == 8192);
	assert(fread(ioc_eeprom + 0x4000, 1, 8192, fhandle) == 8192);
	assert(fread(ioc_eeprom + 0x2000, 1, 8192, fhandle) == 8192);
	assert(fread(ioc_eeprom + 0x6000, 1, 8192, fhandle) == 8192);
	AZ(fclose(fhandle));

	fhandle = fopen("RESHA_EEPROM.bin", "rb");
	AN(fhandle);
	assert(fread(resha_eeprom + 0x0000, 1, 32768, fhandle) == 32768);
	AZ(fclose(fhandle));

	if (1) {
		// Patch UART delay-loop much shorter
		ioc_eeprom[0x64] = 0;

		// Patch UART delay-loop much shorter
		ioc_eeprom[0x101] = 0;
		ioc_eeprom[0x102] = 0;
		ioc_eeprom[0x103] = 4;

		// Patch UART delay-loop much shorter
		ioc_eeprom[0x136] = 0;

		// Patch UART delay-loop much shorter
		ioc_eeprom[0x33c] = 0;

		// Patch UART delay-loop much shorter
		ioc_eeprom[0x34c] = 0;

		// Patch RESHA delay routine
		resha_eeprom[0x458c] = 0;
		resha_eeprom[0x458d] = 1;

		// See also patches in dma_write()
	}

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

	insert_jump(0x80000fa0, 0x80000fda); // RESHA EEProm Interface ...

	insert_jump(0x80001170, 0x8000117c); // RESHA VEM sub-tests
	insert_jump(0x8000117c, 0x80001188); // RESHA LANCE sub-tests
	//insert_jump(0x80001188, 0x80001194); // RESHA DISK SUB-TESTs
	//insert_jump(0x80001194, 0x800011a0); // RESHA TAPE SUB-TESTs

	//insert_jump(0x800011c0, 0x800014d0); // Local interrupts

	insert_jump(0x80001502, 0x800015ce); // Illegal reference protection
	insert_jump(0x800015f2, 0x8000166c); // I/O bus parity
	insert_jump(0x8000169c, 0x800016d8); // I/O bus spurious interrupts
	insert_jump(0x80001700, 0x80001746); // Temperature sensors
	insert_jump(0x80001774, 0x800017f8); // IOC diagnostic processor
	insert_jump(0x8000181c, 0x8000185c); // Power margining
	insert_jump(0x80001880, 0x8000197c); // Clock margining
	insert_jump(0x80001982, 0x80001992); // final check

	// Y2K
	ioc_eeprom[0x3825] = '2';
	ioc_eeprom[0x3826] = '0';

	// Local interrupts test
	insert_jump(0x800011dc, 0x800011fc); // XXX: Where does vector 0x50 come from ?!
	insert_jump(0x8000127a, 0x80001298); // XXX: Where does vector 0x51 come from ?!
	insert_jump(0x80001358, 0x80001470); // XXX: Where does vector 0x52 come from ?!

	// RESHA TAPE SCSI sub-tests
	resha_eeprom[0x139b] = 2;
	resha_eeprom[0x13a1] = 2;
	resha_eeprom[0x14cd] = 2;

	m68k_init();
	m68k_set_cpu_type(IOC_CPU_TYPE);
	m68k_pulse_reset();

	reg_800[3] = 0x7;
	while(1)
	{
		if (irq_level != last_irq_level) {
			last_irq_level = irq_level;
			m68k_set_irq(last_irq_level);
			trace(TRACE_68K, "IRQ level %x\n", last_irq_level);
			reg_800[3] &= ~7;
			reg_800[3] |= (~irq_level) & 7;
		}
		r1000sim->simclock += 100 * m68k_execute(1);
		callout_poll(r1000sim);
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
