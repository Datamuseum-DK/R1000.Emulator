#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

#include "r1000.h"
#include "elastic.h"
#include "ioc.h"

static pthread_t ioc_cpu;

static void
ioc_start_thread(void)
{
	AZ(pthread_create(&ioc_cpu, NULL, main_ioc, NULL));
}

void v_matchproto_(cli_func_f)
cli_ioc_reset(struct cli *cli)
{

	if (cli->help) {
		cli_usage(cli, "\n\tReset IOC CPU\n");
		return;
	}
	if (cli_n_args(cli, 0))
		return;

	cli->ac--;
	cli->av++;

	ioc_start_thread();
}

static const struct cli_cmds cli_ioc_cmds[] = {
	{ "diagbus",		cli_ioc_diag },
	{ "reset",		cli_ioc_reset },
	{ "ioc",		cli_ioc_main },
	{ "scsi_disk",		cli_scsi_disk },
	{ "scsi_tape",		cli_scsi_tape },
	{ "syscall",		cli_ioc_syscall },
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
}

