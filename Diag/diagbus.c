/*
 * The DIAG BUS
 */

#include <string.h>

#include "r1000.h"
#include "ioc.h"
#include "elastic.h"

struct elastic *diag_elastic;

#define BOARD_TABLE \
	BOARD("SEQ", "seq", 2) \
	BOARD("FIU", "fiu", 3) \
	BOARD("IOC", "ioc", 4) \
	BOARD("TYP", "typ", 6) \
	BOARD("VAL", "val", 7) \
	BOARD("MEM0", "mem0", 14) \
	BOARD("MEM2", "mem0", 12)

static const struct boards {
	const char *upper;
	const char *lower;
} boards[16] = {
#define BOARD(upper, lower, address) [address] = { upper, lower },
BOARD_TABLE
#undef BOARD
};


void v_matchproto_(cli_func_f)
cli_diag(struct cli *cli)
{

	if (cli->help || cli->ac == 1) {
		cli_io_help(cli, "diag", 0, 1);
		cli_diag_experiment(cli);
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
		cli_printf(cli, "<<%s>>\n", cli->av[0]);
		cli_unknown(cli);
		break;
	}
}
