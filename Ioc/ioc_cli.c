#include <fcntl.h>
#include <pthread.h>
#include <string.h>

#include "r1000.h"
#include "elastic.h"
#include "ioc.h"

static pthread_t ioc_cpu;

void v_matchproto_(cli_func_f)
cli_ioc_reset(struct cli *cli)
{
	if (cli->help) {
		cli_io_help(cli, "IOC reset", 0, 1);
		return;
	}
	cli->ac--;
	cli->av++;

	AZ(pthread_create(&ioc_cpu, NULL, main_ioc, NULL));
}
