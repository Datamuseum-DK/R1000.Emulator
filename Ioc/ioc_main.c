#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/endian.h>
#include <time.h>
#include "r1000.h"
#include "ioc.h"
#include "m68k.h"

#define IOC_CPU_TYPE	M68K_CPU_TYPE_68020

/* Prototypes */
void exit_error(char* fmt, ...);

static uint8_t ioc_eeprom[32768];
static uint8_t resha_eeprom[32768];
static uint8_t ram[1<<19];

uintmax_t ioc_nins;
unsigned int ioc_pc;

/* Exit with an error message.  Use printf syntax. */
void exit_error(char* fmt, ...)
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
	return be16dec(space + address);
}

static unsigned int  v_matchproto_(memfunc_f)
rdlong(const char *op, uint8_t *space, unsigned int address, unsigned int data)
{
	(void)op;
	(void)data;
	return be32dec(space + address);
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
	be16enc(space + address, data);
	return (0);
}

static unsigned int  v_matchproto_(memfunc_f)
wrlong(const char *op, uint8_t *space, unsigned int address, unsigned int data)
{
	(void)op;
	be32enc(space + address, data);
	return (0);
}

/**********************************************************************/

static unsigned int
io_registers(const char *op, unsigned int address, memfunc_f *func, unsigned int value)
{
	unsigned int retval = 0;
	static uint8_t reg_400[4];
	static uint8_t reg_800[4];

	if (address == 0xfffff400) {
		retval = func(op, reg_400, address & 3, value);
	} else if (0xfffff800 == (address & ~3)) {
		reg_800[3] |= 0x20;
		retval = func(op, reg_800, address & 3, value);
		reg_800[3] |= 0x20;
	} else {
		exit_error("Attempted io_reg memory at address %08x", address & ~3);
	}

	return (retval);
}


/**********************************************************************/

static unsigned int
io_resha_eeprom(const char *op, unsigned int address, memfunc_f *func, unsigned int value)
{
	unsigned int retval = 0, u;
	static uint8_t eeprom_page[4];

	(void)func;
	if (address == 0x9303e00a)
		retval = func(op, eeprom_page, address&3, value);
	if (0x9303e300 == (address & ~0xff)) {
		u = (eeprom_page[2]<<8) | (address & 0xff);
		u &= 0x7fff;
		if (op[0] == 'R')
			retval = resha_eeprom[u];
	}

	return (retval);
}

/**********************************************************************/

static unsigned int
mem(const char *op, unsigned int address, memfunc_f *func, unsigned int value)
{
	if (address < 0x8000 && ioc_nins < 2)
		return func(op, ioc_eeprom, address & 0x7fffffff, value);
	if (address <= sizeof ram)
		return func(op, ram, address & 0x7fffffff, value);
	if (0x80000000 <= address && address <= 0x80007fff)
		return func(op, ioc_eeprom, address & 0x7fffffff, value);
	if (0x9303e000 == (address & ~0xfff))	// Something EEPROM related on RESHAS
		return io_resha_eeprom(op, address, func, value);
	if (0xa1000000 == (address & ~0xffff))
		return (0);
	if (0xffff8000 == (address & ~0xfff))
		return io_rtc(op, address, func, value);
	if (0xffff9000 == (address & ~0xfff))
		return io_console_uart(op, address, func, value);
	if (0xffffa000 == (address & ~0xfff))
		return io_duart(op, address, func, value);
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
	return mem("Rw", address, rdword, 0);
}

unsigned int
m68k_read_memory_32(unsigned int address)
{
	return mem("Rl", address, rdlong, 0);
}

unsigned int
m68k_read_disassembler_32(unsigned int address)
{
	return mem("Rl", address, rdlong, 0);
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
void make_hex(char* buff, unsigned int pc, unsigned int length)
{
	char* ptr = buff;

	for(;length>0;length -= 2)
	{
		sprintf(ptr, "%04x", m68k_read_memory_16(pc));
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

	ioc_pc = pc;
	ioc_nins++;
	if (r1000sim->do_trace) {
		instr_size = m68k_disassemble(buff, pc, IOC_CPU_TYPE);
		make_hex(buff2, pc, instr_size);
		trace(1, "E %03x: %-20s: %s\n", pc, buff2, buff);
	}
	if (pc == 0x800000b4) {
		a6 =  m68k_get_reg(NULL, M68K_REG_A6);
		printf("Self test at 0x%x failed\n", a6);
		exit(2);
	}
}

static void
insert_jump(unsigned int from, unsigned int to)
{

	from &= 0x7fffffff;
	ioc_eeprom[from] = 0x4e;
	ioc_eeprom[from+1L] = 0xf9;
	be32enc(ioc_eeprom + from + 2, to);
}

static void
insert_return(unsigned int from)
{
	from &= 0x7fffffff;
	ioc_eeprom[from] = 0x4e;
	ioc_eeprom[from+1L] = 0x75;
}

/* The main loop */
void *
main_ioc(void *priv)
{
	FILE* fhandle;

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

	insert_jump(0x800001e4, 0x8000021a); // EEPROM CHECKSUM
	insert_jump(0x800003a4, 0x80000546); // 512k RAM Test
	insert_jump(0x80000568, 0x800007d0); // Parity
	insert_jump(0x800007f4, 0x800009b2); // I/O Bus control
	insert_jump(0x800009da, 0x80000a4a); // I/O Bus map parity
	insert_jump(0x80000a74, 0x80000b8a); // I/O bus transactions
	insert_jump(0x80000ba2, 0x80000bf2); // PIT
	//insert_jump(0x80000c1a, 0x80000d20); // Modem DUART channel
	//insert_jump(0x80000d4e, 0x80000dd6); // Diagnostic DUART channel
	//insert_jump(0x80000dfc, 0x80000ec4); // Clock / Calendar
	insert_jump(0x80000fa0, 0x80000fda); // RESHA EEProm Interface ...
	insert_return(0x80001122); // RESHA based selftests
	insert_jump(0x800011c0, 0x800014d0); // Local interrupts
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

	m68k_init();
	m68k_set_cpu_type(IOC_CPU_TYPE);
	m68k_pulse_reset();

	while(1)
	{
		// Our loop requires some interleaving to allow us to update the
		// input, output, and nmi devices.

		// Values to execute determine the interleave rate.
		// Smaller values allow for more accurate interleaving with multiple
		// devices/CPUs but is more processor intensive.
		// 100000 is usually a good value to start at, then work from there.

		// Note that I am not emulating the correct clock speed!
		m68k_execute(1);
	}

	return NULL;
}
