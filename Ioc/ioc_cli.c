#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

#include "Infra/r1000.h"
#include "Ioc/ioc.h"

static const struct cli_cmds cli_ioc_cmds[] = {
	{ "breakpoint",		cli_ioc_breakpoint },
	{ "dump",		cli_ioc_dump },
	{ "maxins",		cli_ioc_maxins },
	{ "memtrace",		cli_ioc_memtrace },
	{ "reset",		cli_ioc_reset },
	{ "syscall",		cli_ioc_syscall },
	{ "step",		cli_ioc_step },
	{ "stop",		cli_ioc_stop },
	{ "start",		cli_ioc_start },
	{ NULL,			NULL },
};

void v_matchproto_(cli_func_f)
cli_ioc(struct cli *cli)
{

	cli_redispatch(cli, cli_ioc_cmds);
}
