
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <fnmatch.h>

#include "Infra/r1000.h"
#include "Infra/vend.h"
#include "Ioc/ioc_scsi.h"

#define DFS_NAME_LEN 30
#define DFS_DIRENT_LEN 64

struct dfs_dirent {
	char		name[DFS_NAME_LEN + 1];
	uint16_t	hash;
	uint16_t	used_sec;
	uint16_t	alloc_sec;
	uint16_t	first_sec;
	uint16_t	time;
	uint16_t	date;
	uint16_t	flags;

	uint8_t		data[64];
};

#define N_DIR_STANZA 8

struct dfs_sb {
	struct {
		uint16_t	magic;
		uint16_t	freeblk;
		uint16_t	dirstanza[N_DIR_STANZA];
	} sec4[1];
	unsigned next_sz;
	unsigned next_sec;
	unsigned next_de;
};

static uint8_t *
dfs_read_sect(unsigned secno)
{
	uint8_t *retval;
	const struct scsi_dev *sd;

	sd = scsi_d[0].dev[0];
	assert(sd);
	assert(sd->map);
	assert(((secno + 1ULL) << 10) <= sd->map_size);

	retval = sd->map + (secno << 10);
	return (retval);
}

static struct dfs_sb *
cli_dfs_read_sb(void)
{
	struct dfs_sb *dsb;
	uint8_t *bp;
	unsigned u;

	dsb = calloc(sizeof *dsb, 1);
	AN(dsb);
	bp = dfs_read_sect(4);
	dsb->sec4->magic = vbe16dec(bp);
	dsb->sec4->freeblk = vbe16dec(bp + 2);
	for (u = 0; u < N_DIR_STANZA; u++)
		dsb->sec4->dirstanza[u] = vbe16dec(bp + 4 + 2 * u);

	assert(dsb->sec4->magic == 0x7fed);
	return (dsb);
}

static int
cli_dfs_iter_dirent(void **priv, struct dfs_dirent *de)
{
	struct dfs_sb *dsb;
	uint8_t *bp;
	unsigned u;

	if (*priv == NULL) {
		dsb = cli_dfs_read_sb();
		AN(dsb);
		*priv = dsb;
	} else {
		dsb = *priv;
	}
	do {
		if (dsb->next_sz == N_DIR_STANZA) {
			free(dsb);
			*priv = NULL;
			return (0);
		}
		bp = dfs_read_sect(
		    dsb->sec4->dirstanza[dsb->next_sz] + dsb->next_sec
		);
		bp += dsb->next_de;
		dsb->next_de += DFS_DIRENT_LEN;
		if (dsb->next_de == (1<<10)) {
			dsb->next_de = 0;
			dsb->next_sec++;
		}
		if (dsb->next_sec == (1<<8)) {
			dsb->next_sec = 0;
			dsb->next_sz++;
		}
	} while (*bp == 0xff);

	memcpy(de->data, bp, sizeof de->data);

	memcpy(de->name, bp, DFS_NAME_LEN);
	de->name[DFS_NAME_LEN] = '\0';

	de->hash = vbe16dec(bp + 30);
	de->used_sec = vbe16dec(bp + 32);
	de->alloc_sec = vbe16dec(bp + 34);
	de->first_sec = vbe16dec(bp + 36);
	for (u = 38; u < 58; u++)
		assert(bp[u] == 0);
	de->time = vbe16dec(bp + 58);
	de->date = vbe16dec(bp + 60);
	de->flags = vbe16dec(bp + 62);
	return(1);
}

static const char * const month[16] = {
	"0x0", "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL",
	"AUG", "SEP", "OCT", "NOV", "DEC", "0xd", "0xe", "0xf",
};

static void
cli_dfs_render(struct cli *cli, struct dfs_dirent *de)
{
	unsigned dd, mm, yy;

	cli_printf(cli, "%-30.30s ", de->name);
	cli_printf(cli, "%04x ", de->hash);
	cli_printf(cli, "%04x ", de->used_sec);
	cli_printf(cli, "%04x ", de->alloc_sec);
	cli_printf(cli, "%04x ", de->first_sec);
	cli_printf(cli, "%02d:", (de->time<<1) / 3600);
	cli_printf(cli, "%02d:", ((de->time<<1) / 60) % 60);
	cli_printf(cli, "%02d ", (de->time<<1) % 60);

	// ((YYYY - 1901) << 9) | (MM << 5) | DD
	dd = de->date & 0x1f;
	mm = (de->date >> 5) & 0x0f;
	yy = (de->date >> 9) + 1;
	cli_printf(cli, "%02d-%s-%02d ", dd, month[mm], yy);
	cli_printf(cli, "%04x\n", de->flags);
}

static int
decompar(const void *a, const void *b)
{
	return (
	    strcmp(
		((const struct dfs_dirent*)a)->name,
		((const struct dfs_dirent*)b)->name
	    )
	);
}

static void v_matchproto_(cli_func_f)
cli_dfs_dir(struct cli *cli)
{
	void *iter = NULL;
	struct dfs_dirent de[4096], *dep;
	int nde = 0, i;

	if (cli->help) {
		cli_printf(cli, "dfs dir [glob…]\n");
		cli_printf(cli,
		    "\tList DFS directory optionally matching pattern(s).\n");
		return;
	}

	dep = de;
	while(cli_dfs_iter_dirent(&iter, dep)) {
		if (cli->ac == 1) {
			dep++;
			nde++;
			continue;
		}
		for (i = 1; i < cli->ac; i++) {
			if (fnmatch(cli->av[i], dep->name, 0))
				continue;
			dep++;
			nde++;
			break;
		}
	}
	qsort(de, nde, sizeof de[0], decompar);
	for (i = 0; i < nde; i++)
		cli_dfs_render(cli, de + i);
}

static const struct cli_cmds cli_dfs_cmds[] = {
	{ "dir",		cli_dfs_dir, "[glob…]" },
	{ NULL,			NULL },
};

void v_matchproto_(cli_func_f)
cli_dfs(struct cli *cli)
{

	if (cli->ac > 1 || cli->help) {
		cli->ac--;
		cli->av++;
		cli_dispatch(cli, cli_dfs_cmds);
	}
}
