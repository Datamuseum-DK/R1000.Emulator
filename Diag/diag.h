
#define BOARD_TABLE \
	BOARD("SEQ", "seq", 2) \
	BOARD("FIU", "fiu", 3) \
	BOARD("IOC", "ioc", 4) \
	BOARD("TYP", "typ", 6) \
	BOARD("VAL", "val", 7) \
	BOARD("MEM0", "mem0", 14) \
	BOARD("MEM2", "mem0", 12)

struct elastic;
struct experiment;
extern struct elastic *diag_elastic;

struct diproc {
	const char		*upper;
	const char		*lower;
	int			address;
	volatile uint8_t	status;
	struct experiment	*experiment;
};

extern struct diproc diprocs[16];
void DiagBus_Send(struct diproc *dp, unsigned u);

struct experiment;
struct experiment *Load_Experiment_File(struct cli *, const char *filename);

cli_func_f cli_diag_experiment;
cli_func_f cli_diag_check;
