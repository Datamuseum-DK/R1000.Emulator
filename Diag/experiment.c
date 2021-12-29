
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
#include "Diag/diag.h"

struct exp_param {
	VTAILQ_ENTRY(exp_param)		list;
	char				*str;
	uint8_t				at;
	uint8_t				len;
};

struct experiment {
	VTAILQ_HEAD(,exp_param)		params;
	uint8_t				octets[256];
	uint8_t				length;
	uint8_t				sum;
};

static void
add_param(struct experiment *ex, const char *str)
{
	struct exp_param *param;

	param = calloc(sizeof *param, 1);
	AN(param);
	VTAILQ_INSERT_TAIL(&ex->params, param, list);
	param->str = strdup(str);
	AN(param->str)
	param->at = strtoul(str + 1, NULL, 16);
	param->len = strtoul(str + 5, NULL, 16);
}

struct experiment *
Load_Experiment_File(struct cli *cli, const char *filename)
{
	struct experiment *ex;
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
			add_param(ex, buf);
		} else if (sscanf(buf, "%x", &u) == 1) {
			ex->octets[ex->length++] = u;
			ex->sum += u;
		}
	}
	AZ(fclose(f));
	return (ex);
}

static void
Send_Experiment(int addr, const struct experiment *ex)
{
	unsigned u;

	assert(addr >= 0 && addr <= 15);
	DiagBus_Send(&diprocs[addr], 0x1a0 | addr);
	DiagBus_Send(&diprocs[addr], ex->length + 1);
	for (u = 0; u < ex->length; u++)
		DiagBus_Send(&diprocs[addr], ex->octets[u]);
	DiagBus_Send(&diprocs[addr], 0);
	DiagBus_Send(&diprocs[addr], (ex->length + 1 + ex->sum) & 0xff);
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
	diprocs[cli->priv].experiment = ex;
}

void v_matchproto_(cli_func_f)
cli_diag_check(struct cli *cli)
{
	struct diproc *dp;
	struct experiment *ex;
	struct exp_param *ep;
	uint8_t buf[256];
	size_t want, sz, z, w;

	if (cli->help || cli->ac != 1) {
		cli_printf(cli, "\t<board> check\n");
		return;
	}
	dp = &diprocs[cli->priv];
	ex = dp->experiment;
	if (ex == NULL) {
		cli_error(cli, "No active experiment\n");
		return;
	}
	want = ex->octets[0];
	w = 0;
	VTAILQ_FOREACH(ep, &ex->params, list) {
		if (ep->at + ep->len > want)
			want = ep->at + ep->len;
		if (ep->len > w)
			w = ep->len;
	}
	// want -= 0x10;
	DiagBus_Send(dp, 0x120 + cli->priv);
	DiagBus_Send(dp, 0);
	DiagBus_Send(dp, want);

	for (z = 0; z < 2 + want; z += sz)
		sz = elastic_get(diag_elastic, buf + z, sizeof buf - z);

	cli_printf(cli, "Status: %02x (%s)\nMemory:\n",
	    buf[0],
	    (buf[0] & 2) ? "GOOD" : "BAD"
	);
	for (sz = 1; sz < want + 1; sz++) {
		if (sz % 16 == 1)
			cli_printf(cli, "  %02zx: ", sz - 1);
		cli_printf(cli, "%02x", buf[sz]);
		if (sz % 16 == 0)
			cli_printf(cli, "\n");
		else if (sz % 4 == 0)
			cli_printf(cli, " ");
	}
	cli_printf(cli, "\nParameters:\n");
	VTAILQ_FOREACH(ep, &ex->params, list) {
		for (z = 0; z < ep->len; z++)
			cli_printf(cli, "%02x", buf[1 + ep->at + z]);
		for (; z < w; z++)
			cli_printf(cli, "  ");
		cli_printf(cli, "  [%02x:%02x] %s", ep->at, ep->len, ep->str);
	}
}
