#ifndef DIAG_DIAG_H
#define DIAG_DIAG_H

#define BOARD_TABLE(macro) \
	macro(SEQ, seq, 2) \
	macro(FIU, fiu, 3) \
	macro(IOC, ioc, 4) \
	macro(TYP, typ, 6) \
	macro(VAL, val, 7) \
	macro(MEM0, mem0, 12) \
	macro(MEM1, mem1, 13) \
	macro(MEM2, mem2, 14) \
	macro(MEM3, mem3, 15)

#define RESPONSE_TABLE(macro) \
	macro(1, OK) \
	macro(2, LOOPING) \
	macro(3, PAUSED) \
	macro(4, ERROR) \
	macro(5, RESET) \
	macro(6, RUNNING) \
	macro(7, RESERVED1) \
	macro(8, RESERVED2) \
	macro(9, TIMEOUT)

enum diproc_response {
#define RESPONSE(num, name)	DIPROC_RESPONSE_##name = num,
RESPONSE_TABLE(RESPONSE)
#undef RESPONSE
};

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
void DiagBus_Send(const struct diproc *dp, unsigned u);

struct diproc *diagbus_get_board(struct cli *cli, const char *);

struct experiment *Load_Experiment_File(struct cli *, const char *filename);

cli_func_f cli_diproc_experiment;
cli_func_f cli_diproc_status;
cli_func_f cli_diproc_dummy;
void cli_diproc_help_status(struct cli *cli);
void cli_diproc_help_board(struct cli *cli);


#endif /* DIAG_DIAG_H */
