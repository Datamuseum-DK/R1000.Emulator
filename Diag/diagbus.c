/*
 * The DIAG BUS
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "r1000.h"
#include "elastic.h"
#include "Diag/diag.h"

struct elastic *diag_elastic;

struct diproc diprocs[16] = {
#define BOARD(u, l, a) \
	[a] = { \
		.upper = #u, \
		.lower = #l,\
		.address = a,\
		.status = 0,\
		.rxbusy = 0,\
	 },
BOARD_TABLE
#undef BOARD
};

void
DiagBus_Send(struct diproc *dp, unsigned u)
{
	AN(dp);
	AN(dp->upper);
	assert(u <= 0x1ff);
	Trace(trace_diagbus_bytes, "DIAGBUS TX %03x", u);
	uint8_t buf[2];
	buf[0] = u >> 8;
	buf[1] = u & 0xff;
	elastic_put(diag_elastic, buf, 2);
	dp->rxbusy = 11;
	while(dp->rxbusy)
		usleep(1000);
}

void v_matchproto_(cli_func_f)
cli_diag(struct cli *cli)
{
	int state, want_state = -6;

	if (cli->help || cli->ac == 1) {
		cli_io_help(cli, "diag", 0, 1);
		cli_diag_experiment(cli);
		cli_printf(cli, "\t<board> wait [<state>|-<state>]\n");
		(void)cli_elastic(diag_elastic, cli);
		return;
	}

	cli->ac--;
	cli->av++;

	while (cli->ac && !cli->status) {
		if (!cli_elastic(diag_elastic, cli))
			break;
	}
	if (cli->ac == 0)
		return;

	cli->priv = -1;
#define BOARD(upper, lower, address) \
	if (!strcasecmp(cli->av[0], upper)) \
		cli->priv = address;
BOARD_TABLE
#undef BOARD
	if (cli->priv < 0) {
		cli_error(cli, "Unknown board\n");
		return;
	}
	cli->ac--;
	cli->av++;

	while (cli->ac && !cli->status) {
		if (!strcmp(cli->av[0], "experiment")) {
			cli_diag_experiment(cli);
			return;
		}
		if (!strcmp(cli->av[0], "wait")) {
			if (cli->ac > 1)
				want_state = strtoul(cli->av[1], NULL, 0);
			while (1) {
				state = diprocs[cli->priv].status;
				if (want_state == state)
					break;
				if (want_state < 0 && -want_state != state)
					break;
				usleep(100000);
			}
			return;
		}
		cli_printf(cli, "<<%s>>\n", cli->av[0]);
		cli_unknown(cli);
		break;
	}
}
