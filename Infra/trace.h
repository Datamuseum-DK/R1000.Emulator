
#define TRACERS								\
	TRACER(ioc_instructions, "IOC CPU instruction execution")	\
	TRACER(ioc_interrupt, "IOC interrupts")				\
	TRACER(ioc_dma, "IOC DMA")					\
	TRACER(ioc_io, "IOC IO")					\
	TRACER(diagbus_bytes, "All bytes on DiagBus")			\
	TRACER(diagbus_download, "Downloads on DiagBus")		\
	TRACER(diagbus_upload, "Uploads on DiagBus")			\
	TRACER(disk_data, "Data to/from disk(s)")			\
	TRACER(tape_data, "Data to/from tape(s)")			\
	TRACER(scsi_cmd, "SCSI commands, both busses")			\
	TRACER(systemc, "SystemC model")
