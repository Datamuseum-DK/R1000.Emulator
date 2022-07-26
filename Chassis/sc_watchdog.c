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

static pthread_t fido_thread;
static int fido_patience = 60;
static int fido_started = 0;
static int fido_dont_bite = 0;

static void *
fido(void *priv)
{
	const struct ctx *cp;
	void *ctx_iter_priv;
	const struct diagproc_context *dctx;
	uint64_t last_exec = 0, last_instr = 0, last_act = 0;
	uint64_t this_exec, this_instr, this_act;
	struct timespec t1;
	double d, e;

	(void)priv;
	sleep(fido_patience);
	while (1) {
		sleep(fido_patience);
		AZ(clock_gettime(CLOCK_REALTIME, &t1));
		e = sc_when();
		if (e > 0) {
			d = 1e-9 * (t1.tv_nsec - sc_t0.tv_nsec);
			d += (t1.tv_sec - sc_t0.tv_sec);
			printf("SC real time: %.3f\tsim time: %.3f\tratio: %.3f\n",
			    d, e, d / e);
		}

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
		if (fido_dont_bite ||
                    (this_exec > last_exec && this_instr > last_instr)) {
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

	if (cli->help || cli->ac < 2 || cli->ac > 3) {
		Cli_Usage(cli, "[-dont_bite] <seconds>",
		    "Tickle watchdog periodically.");
		return;
	}

	if (!strcmp(cli->av[1], "-dont_bite")) {
		fido_dont_bite = 1;
		if (cli->ac == 2)
			patience = 60;
		else
			patience = strtoul(cli->av[2], NULL, 0);
	} else {
		patience = strtoul(cli->av[1], NULL, 0);
	}
	if (patience < 5) {
		Cli_Error(cli, "Too short patience for fido: %d\n", patience);
		return;
	}
	fido_patience = patience;
	if (!fido_started) {
		AZ(pthread_create(&fido_thread, NULL, fido, NULL));
		fido_started = 1;
	}
}
