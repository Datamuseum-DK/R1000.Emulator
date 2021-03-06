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
#include <string.h>

#include "r1000.h"
#include "ioc.h"

#include "memspace.h"

#include "vend.h"

static uint8_t resha_eeprom[32768];

static void
insert_jump(unsigned int from, unsigned int to)
{

	from &= 0x7fffffff;
	ioc_eeprom_space[from] = 0x4e;
	ioc_eeprom_space[from+1L] = 0xf9;
	vbe32enc(ioc_eeprom_space + from + 2, to);
}

void
ioc_load_eeproms(void)
{
	FILE* fhandle;

	fhandle = fopen("IOC_EEPROM.bin", "rb");
	AN(fhandle);
	assert(fread(ioc_eeprom_space + 0x0000, 1, 8192, fhandle) == 8192);
	assert(fread(ioc_eeprom_space + 0x4000, 1, 8192, fhandle) == 8192);
	assert(fread(ioc_eeprom_space + 0x2000, 1, 8192, fhandle) == 8192);
	assert(fread(ioc_eeprom_space + 0x6000, 1, 8192, fhandle) == 8192);
	AZ(fclose(fhandle));

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
