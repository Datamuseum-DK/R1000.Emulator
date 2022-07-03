/*
 * The DIAG BUS
 */

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Infra/r1000.h"
#include "Infra/elastic.h"
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
BOARD_TABLE(BOARD)
#undef BOARD
};

struct diproc *
diagbus_get_board(struct cli *cli, const char *name)
{
#define BOARD(upper, lower, address) \
	if (!strcasecmp(name, #upper)) \
		return(&diprocs[address]);
BOARD_TABLE(BOARD)
#undef BOARD
	cli_error(cli, "Unknown board '%s'\n", name);
	return (NULL);
}

void
cli_diproc_help_status(struct cli *cli)
{
	cli_printf(cli, "\n\tStatus:\n");
#define RESPONSE(num, name) cli_printf(cli, "\t    %s\n", #name);
	RESPONSE_TABLE(RESPONSE)
#undef RESPONSE
}

void
cli_diproc_help_board(struct cli *cli)
{
	cli_printf(cli, "\n\tBoard:\n");
#define BOARD(upper, lower, address) cli_printf(cli, "\t    %s\n", #upper);
	BOARD_TABLE(BOARD)
#undef BOARD
}


void
DiagBus_Send(const struct diproc *dp, unsigned u)
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

#if 0

static void
diagbus_tx(const struct diproc *dp, struct cli *cli)
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

#endif

static void
cli_diproc_wait(struct cli *cli)
{
	int state, want_state = - (int)DIPROC_RESPONSE_RUNNING, i;
	uint8_t buf[1];
	struct diproc *dp;

	if (cli->help || cli->ac < 2 || cli->ac > 3) {
		cli_usage(cli, "[[-]<status>] <board>",
		    "Wait for DIPROC to reach or leave (-) status");
		if (cli->help == 1) {
			cli_diproc_help_status(cli);
			cli_diproc_help_board(cli);
		}
		return;
	}

	cli->ac--;
	cli->av++;
	if (cli->ac == 2) {
		if (cli->av[0][0] == '-')
			i = 1;
		else
			i = 0;
		want_state = 0;
#define RESPONSE(num, name) \
		if (!strcasecmp(#name, cli->av[0] + i)) \
			want_state = num;
		RESPONSE_TABLE(RESPONSE)
#undef RESPONSE
		if (want_state == 0) {
			cli_error(cli, "Unknown state '%s'\n", cli->av[0]);
			return;
		}
		if (i)
			want_state = -want_state;
		cli->ac--;
		cli->av++;
	}
	dp = diagbus_get_board(cli, cli->av[0]);
	if (dp == NULL)
		return;

	/*
	 * Do first poll via diagbus, to make sure download is complete
	 */
	DiagBus_Send(dp, 0x100 + dp->address);
	assert(1 == elastic_get(diag_elastic, buf, sizeof buf));
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
cli_diagbus(struct cli *cli)
{
	if (cli->help || cli->ac < 2) {
		cli_usage(cli, "<elastic>", "Steer diagbus output");
		(void)cli_elastic(diag_elastic, cli);
		return;
	}
	cli->ac--;
	cli->av++;
	while (cli->ac && !cli->status) {
		if (cli_elastic(diag_elastic, cli))
			continue;
		break;
	}
}

static const struct cli_cmds cli_diproc_cmds[] = {
	{ "dummy",		cli_diproc_dummy },
	{ "experiment",		cli_diproc_experiment },
	{ "status",		cli_diproc_status },
	{ "wait",		cli_diproc_wait },
	{ NULL,			NULL },
};

void v_matchproto_(cli_func_f)
cli_diproc(struct cli *cli)
{

	cli_redispatch(cli, cli_diproc_cmds);
}

void
diagbus_init(void)
{
	diag_elastic = elastic_new(O_RDWR);
	AN(diag_elastic);
}
