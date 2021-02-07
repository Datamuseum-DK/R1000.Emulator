
void v_matchproto_(cli_func_f) cli_ioc_console(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_duart(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_reset(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_main(struct cli *cli);

void *main_ioc(void *priv);

typedef unsigned int memfunc_f(
    const char *op,
    uint8_t *space,
    unsigned int address,
    unsigned int data
);

typedef unsigned int iofunc_f(
    const char *op,
    unsigned int address,
    memfunc_f *func,
    unsigned int data
);

iofunc_f io_console_uart;
iofunc_f io_duart;
iofunc_f io_rtc;
iofunc_f io_scsi_d;
void ioc_console_init(struct sim *);
void ioc_duart_init(struct sim *);

extern uintmax_t ioc_nins;
extern unsigned ioc_pc;
void dma_write(unsigned segment, unsigned address, void *src, unsigned len);

#define IO_TRACE_WRITE(level, prefix)					\
	do {								\
		if (*op == 'W')						\
			trace(level, prefix " %08x %s %08x %x\n",	\
			    ioc_pc, op, address, value);		\
	} while(0)

#define IO_TRACE_READ(level, prefix)					\
	do {								\
		if (*op == 'R')						\
			trace(level, prefix " %08x %s %08x %x\n",	\
			    ioc_pc, op, address, value);		\
	} while(0)
