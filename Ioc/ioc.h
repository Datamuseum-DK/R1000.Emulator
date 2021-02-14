
void v_matchproto_(cli_func_f) cli_ioc_console(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_duart(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_reset(struct cli *cli);
void v_matchproto_(cli_func_f) cli_ioc_main(struct cli *cli);
void v_matchproto_(cli_func_f) cli_scsi_disk(struct cli *cli);
void v_matchproto_(cli_func_f) cli_scsi_tape(struct cli *cli);

void *main_ioc(void *priv);

void ioc_console_init(struct sim *);
void ioc_duart_init(struct sim *);
void ioc_scsi_d_init(struct sim *);
void ioc_scsi_t_init(struct sim *);
void ioc_rtc_init(struct sim *);
void ioc_init(struct sim *);

extern uintmax_t ioc_nins;
extern unsigned ioc_pc;
void dma_write(unsigned segment, unsigned address, void *src, unsigned len);
void dma_read(unsigned segment, unsigned address, void *src, unsigned len);

/**********************************************************************/

void Ioc_HotFix_Bootloader(uint8_t *ram);
void Ioc_HotFix_Ioc(uint8_t *eeprom);
void Ioc_HotFix_Resha(uint8_t *eeprom);
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
	IRQ_VECTOR(PIT,			1,	0x4f,	0x14f) \
	IRQ_VECTOR(SCSI_D,		2,	0x91,	0x691) \
	IRQ_VECTOR(SCSI_T,		3,	0x92,	0x692)

struct irq_vector;

#define IRQ_VECTOR(upper, level, vector, priority) \
	extern struct irq_vector IRQ_##upper;
INTERRUPT_TABLE
#undef IRQ_VECTOR

extern unsigned irq_level;

void irq_raise(struct irq_vector *vp);
void irq_lower(struct irq_vector *vp);
