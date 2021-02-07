
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
iofunc_f io_scsi_d_reg;
iofunc_f io_scsi_d;
iofunc_f io_scsi_t;
void ioc_console_init(struct sim *);
void ioc_duart_init(struct sim *);
void ioc_scsi_d_init(struct sim *);
void ioc_init(struct sim *);

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

/**********************************************************************/

#define INTERRUPT_TABLE \
	IRQ_VECTOR(CONSOLE_BREAK, L67, 0x42) \
	IRQ_VECTOR(SCSI_D, L67, 0x91)

struct irq_vector;

#define IRQ_VECTOR(upper, level, vector) \
	extern struct irq_vector IRQ_##upper;
INTERRUPT_TABLE
#undef IRQ_VECTOR

extern unsigned irq_level;

int irq_raise(struct irq_vector *vp);
int irq_lower(struct irq_vector *vp);
unsigned irq_getvector(void);
