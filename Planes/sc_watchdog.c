#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "r1000.h"
#include "r1000sc_priv.h"
#include "vqueue.h"
#include "context.h"
#include "../Diag/diagproc.h"

#define WATCHDOG_USAGE "Usage:\n\tpatience\n"

static pthread_t fido_thread;
static int fido_patience = 60;
static int fido_started = 0;

static void *
fido(void *priv)
{
	const struct ctx *cp;
	void *ctx_iter_priv;
	const struct diagproc_context *dctx;
	uint64_t last_exec = 0, last_instr = 0;
	uint64_t this_exec, this_instr;

	(void)priv;
	sleep(fido_patience);
	while (1) {
		sleep(fido_patience);
		this_exec = this_instr = 0;
		ctx_iter_priv = NULL;
		while(1) {
			cp = CTX_Iter(&ctx_iter_priv);
			if (cp == NULL)
				break;
			if (strstr(cp->ident, "PROC") == NULL)
				continue;
			dctx = (const void*)(cp+1);
			printf("FIDO barks: %s %s act %ju (%.2f/s) exec %ju (%.2f/s) instr %ju (%.2f/s)\n",
			    cp->kind, cp->ident,
			    (uintmax_t)cp->activations,
			    (double)cp->activations / (double)fido_patience,
			    (uintmax_t)dctx->executions,
			    (double)dctx->executions / (double)fido_patience,
			    (uintmax_t)dctx->instructions,
			    (double)dctx->instructions / (double)fido_patience
			);
			this_exec += dctx->executions;
			this_instr += dctx->instructions;
		}
		if (this_exec > last_exec && this_instr > last_instr) {
			last_exec = this_exec;
			last_instr = this_instr;
			continue;
		}
		if (last_instr == 0)
			finish(9, "SC Watchdog sees no instructions");
		if (last_exec == 0)
			finish(9, "SC Watchdog sees no exections");
		if (this_instr == last_instr)
			finish(8, "SC Watchdog sees instructions stalled");
		assert (this_exec == last_instr);
		finish(8, "SC Watchdog sees executions stalled");
	}
}


void v_matchproto_(cli_func_f)
cli_sc_watchdog(struct cli *cli)
{
	int patience;

	if (cli->help) {
		cli_printf(cli, WATCHDOG_USAGE);
		return;
	}
	if (cli->ac > 1) {
		patience = strtoul(cli->av[1], NULL, 0);
		if (patience < 5) {
			cli_error(cli, "Too short patience for fido: %d\n", patience);
			return;
		}
		fido_patience = patience;
	}
	cli_printf(cli, "Fidos patience is %d seconds\n", fido_patience);
	if (!fido_started) {
		AZ(pthread_create(&fido_thread, NULL, fido, NULL));
		fido_started = 1;
	}
}
