/*
 * The DIAG BUS
 */

#include "r1000.h"
#include "ioc.h"
#include "elastic.h"

struct elastic *diag_elastic;

void v_matchproto_(cli_func_f)
cli_diag(struct cli *cli)
{

	if (cli->help) {
		cli_io_help(cli, "IOC diagbus", 0, 1);
		(void)cli_elastic(diag_elastic, cli);
		return;
	}

	cli->ac--;
	cli->av++;

	while (cli->ac && !cli->status) {
		if (cli_elastic(diag_elastic, cli))
			continue;
		cli_unknown(cli);
		break;
	}
}
