#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

#include "r1000.h"
#include "ioc.h"

static const struct cli_cmds cli_ioc_cmds[] = {
	{ "diagbus",		cli_ioc_diag },
	{ "reset",		cli_ioc_reset },
	{ "scsi_disk",		cli_scsi_disk },
	{ "scsi_tape",		cli_scsi_tape },
	{ "syscall",		cli_ioc_syscall },
	{ "step",		cli_ioc_step },
	{ "stop",		cli_ioc_stop },
	{ "start",		cli_ioc_start },
	{ NULL,			NULL },
};

void v_matchproto_(cli_func_f)
cli_ioc(struct cli *cli)
{

	if (cli->ac > 1 || cli->help) {
		cli->ac--;
		cli->av++;
		cli_dispatch(cli, cli_ioc_cmds);
	}
	cli_ioc_state(cli);
}

