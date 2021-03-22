
void v_matchproto_(cli_func_f) cli_ioc(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_breakpoint(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_console(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_diag(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_dump(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_maxins(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_memtrace(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_modem(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_reset(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_start(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_state(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_step(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_stop(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_syscall(struct cli *cli);
void v_matchproto_(cli_func_f) cli_scsi_disk(struct cli *cli);
void v_matchproto_(cli_func_f) cli_scsi_tape(struct cli *cli);

void *main_ioc(void *priv);

void ioc_debug_init(void);
void ioc_console_init(void);
void ioc_duart_init(void);
void ioc_scsi_d_init(void);
void ioc_scsi_t_init(void);
void ioc_rtc_init(void);
void ioc_init(void);

extern uintmax_t ioc_nins;
extern unsigned ioc_pc;
extern unsigned ioc_fc;
void dma_write(unsigned segment, unsigned address, void *src, unsigned len);
void dma_read(unsigned segment, unsigned address, void *src, unsigned len);

/**********************************************************************/

void ioc_stop_cpu(void);
void ioc_breakpoint_check(uint32_t adr);
void ioc_breakpoint_rpn(uint32_t, const char *);
void ioc_dump_registers(unsigned lvl);
void ioc_dump_cpu_regs(struct vsb *vsb);
void ioc_dump_core(const char *);

/**********************************************************************/

void Ioc_HotFix_Bootloader(uint8_t *ram);
void Ioc_HotFix_Ioc(void);
void Ioc_HotFix_Resha(void);
void Ioc_HotFix_Kernel(uint8_t *ram);

/**********************************************************************/

/*
 * CONSOLE.RXRDY & CONSOLE.TXRDY are unfinished.
 * TXRDY has higher priority than RXRDY, yet the test at 0x800012cc
 * expects to see vector 0x45 instead of 0x49.
 * Swapping the two vectors (tx=45,rx=49) causes kernel interrupt storm.
 * Artificially lowering TXRDY's priority makes the test pass for now.
 */

#define INTERRUPT_TABLE \
	IRQ_VECTOR(CONSOLE_BREAK,	L67,	0x42,	0x642) \
	IRQ_VECTOR(DIAG_BUS_RXRDY,	L67,	0x44,	0x644) \
	IRQ_VECTOR(CONSOLE_RXRDY,	L67,	0x45,	0x645) \
	IRQ_VECTOR(MODEM_RXRDY,		L67,	0x46,	0x646) \
	IRQ_VECTOR(DIAG_BUS_TXRDY,	1,	0x48,	0x148) \
	IRQ_VECTOR(CONSOLE_TXRDY,	1,	0x49,	0x1149) \
	IRQ_VECTOR(MODEM_TXRDY,		L67,	0x4b,	0x646) \
	IRQ_VECTOR(PIT,			1,	0x4f,	0x14f) \
	IRQ_VECTOR(SCSI_D,		2,	0x91,	0x691) \
	IRQ_VECTOR(SCSI_T,		3,	0x92,	0x692)

struct irq_vector;

#define IRQ_VECTOR(upper, level, vector, priority) \
	extern struct irq_vector IRQ_##upper;
INTERRUPT_TABLE
#undef IRQ_VECTOR

extern unsigned irq_level;

unsigned irq_getvector(unsigned int arg);

void irq_raise(struct irq_vector *vp);
void irq_lower(struct irq_vector *vp);

void ioc_load_eeproms(void);

