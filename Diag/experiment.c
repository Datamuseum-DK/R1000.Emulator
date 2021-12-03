
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "r1000.h"
#include "elastic.h"
#include "vqueue.h"

struct exp_param {
	VTAILQ_ENTRY(exp_param)		list;
	char				*str;
};

struct experiment {
	VTAILQ_HEAD(,exp_param)		params;
	uint8_t				octets[256];
	uint8_t				length;
	uint8_t				sum;
};

struct experiment *
Load_Experiment_File(struct cli *cli, const char *filename)
{
	struct experiment *ex;
	struct exp_param *param;
	char buf[BUFSIZ];
	FILE *f;
	unsigned u;

	f = fopen(filename, "r");
	if (f == NULL) {
		cli_error(cli, "Cannot open `%s`: %s\n",
		    filename, strerror(errno));
		return (NULL);
	}

	ex = calloc(sizeof *ex, 1);
	AN(ex);
	VTAILQ_INIT(&ex->params);
	while (fgets(buf, sizeof buf, f) != NULL) {
		if(buf[0] == 'P') {
			param = calloc(sizeof *param, 1);
			AN(param);
			VTAILQ_INSERT_TAIL(&ex->params, param, list);
			param->str = strdup(buf);
			AN(param->str)
		} else if (sscanf(buf, "%x", &u) == 1) {
			ex->octets[ex->length++] = u;
			ex->sum += u;
		}
	}
	AZ(fclose(f));
	return (ex);
}

static void
DiagBus_Send(unsigned u)
{
	printf("diagbus TX %02x\n", u);
	Trace(trace_diagbus_bytes, "DIAGBUS TX %03x", u);
	uint8_t buf[2];
	buf[0] = u >> 8;
	buf[1] = u & 0xff;
	elastic_put(diag_elastic, buf, 2);
	usleep(100000);
}

static void
Send_Experiment(int addr, const struct experiment *ex)
{
	unsigned u;

	assert(addr >= 0 && addr <= 15);
	DiagBus_Send(0x1a0 | addr);
	DiagBus_Send(ex->length + 1);
	for (u = 0; u < ex->length; u++)
		DiagBus_Send(ex->octets[u]);
	DiagBus_Send(0);
	DiagBus_Send((ex->length + 1 + ex->sum) & 0xff);
}

void v_matchproto_(cli_func_f)
cli_diag_experiment(struct cli *cli)
{
	struct experiment *ex;

	if (cli->help || cli->ac == 1) {
		cli_printf(cli, "\t<board> experiment <filename>\n");
		return;
	}
	cli->ac--;
	cli->av++;
	ex = Load_Experiment_File(cli, cli->av[0]);
	if (ex == NULL)
		return;
	usleep(100000);
	Send_Experiment(cli->priv, ex);
}
