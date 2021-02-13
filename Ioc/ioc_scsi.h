
struct scsi_dev;
struct scsi;

typedef int scsi_func_f(struct scsi_dev *, uint8_t *cdb, unsigned dst);
#define IOC_SCSI_OK		0
#define IOC_SCSI_ERROR		-1

struct scsi_dev {
	scsi_func_f		* const * funcs;
	struct scsi		*ctl;
	int			is_tape;
	uint8_t			sense_3[36];
	uint8_t			sense_4[32];
	int			fd;
	uint8_t			*map;
	size_t			map_size;

	size_t			tape_head;
};

struct scsi {
	const char		*name;
	struct irq_vector	*irq_vector;
	pthread_mutex_t		mtx;
	pthread_cond_t		cond;
	pthread_t		thr;
	uint8_t			regs[32];
	unsigned int		dma;
	struct scsi_dev		*dev[7];
};

void trace_scsi_dev(struct scsi_dev *dev, const char *cmt);
extern struct scsi scsi_t[1];
extern struct scsi scsi_d[1];

