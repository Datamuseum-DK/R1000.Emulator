/*-
 * Copyright (c) 2021 Poul-Henning Kamp
 * All rights reserved.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#include "r1000.h"
#include "context.h"

static const char *context_filename = "/critter/_ctx";
static ssize_t context_size = 0;

static int context_fd = -1;
static uint8_t *context_ptr = NULL;
static const uint8_t *context_end = NULL;

static void
ctx_init(void)
{
	ssize_t sz, szl;
	char buf[BUFSIZ];
	void *ptr;

	context_fd = open(context_filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
	assert(context_fd > 0);
	context_size = 16 << 20;

	memset(buf, 0, sizeof buf);
	for (szl = 0; szl < context_size; ) {
		sz = sizeof buf;
		if (sz + szl > context_size)
			sz = context_size - szl;
		sz = write(context_fd, buf, sz);
		assert(sz > 0);
		szl += sz;
	}
	ptr = mmap(NULL, context_size, PROT_READ|PROT_WRITE,
	    MAP_SHARED | MAP_NOSYNC, context_fd, 0);
	assert(ptr != MAP_FAILED);
	context_ptr = ptr;
	context_end = context_ptr + context_size;
}

void *
CTX_Get(const char *kind, const char *ident, uint32_t length)
{

	struct ctx *ctx;

	if (!context_size)
		ctx_init();

	assert(sizeof *ctx == 128);
	assert(strlen(kind) + 1 <= sizeof ctx->kind);
	assert(strlen(ident) + 1 <= sizeof ctx->ident);
	if (length & 0xf) {
		length |= 0xf;
		length += 1;
	}
	assert(context_ptr + length <= context_end);
	ctx = (void*)context_ptr;
	context_ptr += length;
	AN(ctx);
	ctx->magic = CTX_MAGIC;
	ctx->length = length;
	strcpy(ctx->kind, kind);
	strcpy(ctx->ident, ident);
	return (ctx);
}
