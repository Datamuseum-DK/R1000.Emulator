#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Infra/r1000.h"
#include "Chassis/r1000sc_priv.h"
#include "Infra/context.h"
#include "Diag/diagproc.h"

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
	uint64_t last_exec = 0, last_instr = 0, last_act = 0;
	uint64_t this_exec, this_instr, this_act;

	(void)priv;
	sleep(fido_patience);
	while (1) {
		sleep(fido_patience);
		this_exec = this_instr = this_act = 0;
		ctx_iter_priv = NULL;
		while(1) {
			cp = CTX_Iter(&ctx_iter_priv);
			if (cp == NULL)
				break;
			if (strstr(cp->ident, "PROC") == NULL)
				continue;
			dctx = (const void*)(cp+1);
			printf("FIDO barks: %s act %ju mcs51 %ju exp %ju\n",
			    cp->ident,
			    (uintmax_t)cp->activations,
			    (uintmax_t)dctx->instructions,
			    (uintmax_t)dctx->executions
			);
			this_act += cp->activations;
			this_exec += dctx->executions;
			this_instr += dctx->instructions;
		}
		printf("FIDO rate: act %.2f mcs51 %.2f exp %.2f (/s)\n",
		    (double)(this_act - last_act) / (double)fido_patience,
		    (double)(this_instr - last_instr) / (double)fido_patience,
		    (double)(this_exec - last_exec) / (double)fido_patience
		);
		if (this_exec > last_exec && this_instr > last_instr) {
			last_act = this_act;
			last_exec = this_exec;
			last_instr = this_instr;
		} else if (last_instr == 0) {
			finish(9, "SC Watchdog have seen no mcs51 activity");
		} else if (last_exec == 0) {
			finish(9, "SC Watchdog have seen no exp activity");
		} else if (this_instr == last_instr) {
			finish(8, "SC Watchdog sees DIPROC mcs51 stalled");
		} else {
			assert (this_exec == last_exec);
			finish(8, "SC Watchdog sees DIPROC exp stalled");
		}
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
