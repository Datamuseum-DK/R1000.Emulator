/*
 * The DIAG BUS
 */

#include <fcntl.h>
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
}

static void
diagbus_tx(struct diproc *dp, struct cli *cli)
{
	unsigned nonet;
	unsigned sum = 0;

	while (cli->ac > 1) {
		cli->ac--;
		cli->av++;
		if (!strcmp(cli->av[0], "{")) {
			sum = 0;
		} else if (!strcmp(cli->av[0], "}")) {
			DiagBus_Send(dp, sum & 0xff);
		} else {
			nonet = strtoul(cli->av[0], NULL, 0);
			sum += nonet;
			assert(nonet < 0x200);
			DiagBus_Send(dp, nonet);
		}
	}
}

static void
diagbus_wait(struct cli *cli)
{
	int state, want_state = -6;
	uint8_t buf[1];
	size_t sz;
	struct diproc *dp;

	if (cli->ac > 1)
		want_state = strtoul(cli->av[1], NULL, 0);
	dp = &diprocs[cli->priv];
	/*
	 * Do first poll via diagbus, to make sure download is complete
	 */
	DiagBus_Send(dp, 0x100 + cli->priv);
	sz = elastic_get(diag_elastic, buf, sizeof buf);
	state = buf[0] & 0xf;
	while (1) {
		if (want_state == state)
			break;
		if (want_state < 0 && -want_state != state)
			break;
		usleep(100000);
		state = dp->status;
	}
	if (want_state != state)
		cli_printf(cli, "State is 0x%02x\n", state);
	return;
}

void v_matchproto_(cli_func_f)
cli_diag(struct cli *cli)
{

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

	cli->priv = 255;
#define BOARD(upper, lower, address) \
	if (!strcasecmp(cli->av[0], upper)) \
		cli->priv = address;
BOARD_TABLE
#undef BOARD
	if (cli->priv > 0xf) {
		cli_error(cli, "Unknown board (0x%jx)\n", cli->priv);
		return;
	}
	cli->ac--;
	cli->av++;

	while (!diprocs[cli->priv].status)
		usleep(10000);

	while (cli->ac && !cli->status) {
		if (!strcmp(cli->av[0], "experiment")) {
			cli_diag_experiment(cli);
			return;
		}
		if (!strcmp(cli->av[0], "check")) {
			cli_diag_check(cli);
			return;
		}
		if (!strcmp(cli->av[0], "wait")) {
			diagbus_wait(cli);
			return;
		}
		if (!strcmp(cli->av[0], "tx")) {
			diagbus_tx(&diprocs[cli->priv], cli);
			return;
		}
		cli_printf(cli, "<<%s>>\n", cli->av[0]);
		cli_unknown(cli);
		break;
	}
}

void
diagbus_init(void)
{
	diag_elastic = elastic_new(O_RDWR);
	AN(diag_elastic);
}
