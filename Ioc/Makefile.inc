
OBJS += Ioc/ioc_cli.o
Ioc/ioc_cli.o: \
    Ioc/ioc_cli.c \
    Ioc/ioc.h \
    ${R1000DEP}

OBJS += Ioc/ioc_debug.o
Ioc/ioc_debug.o: \
    Ioc/ioc_debug.c \
    Musashi/m68k.h \
    Ioc/ioc.h \
    Infra/vsb.h \
    Ioc/memspace.h \
    Ioc/_memcfg.h \
    ${R1000DEP}

OBJS += Ioc/ioc_duart.o
Ioc/ioc_duart.o: \
    Ioc/ioc_duart.c \
    Ioc/ioc.h \
    Infra/elastic.h \
    Ioc/memspace.h \
    Diag/diag.h \
    ${R1000DEP}

OBJS += Ioc/ioc_eeproms.o
Ioc/ioc_eeproms.o: \
    Ioc/ioc_eeproms.c \
    Ioc/ioc.h \
    Ioc/memspace.h \
    Infra/vend.h \
    Chassis/r1000sc.h \
    ${R1000DEP}

OBJS += Ioc/ioc_pnlreg.o
Ioc/ioc_pnlreg.o: \
    Ioc/ioc_pnlreg.c \
    Ioc/memspace.h \
    Infra/vend.h \
    ${R1000DEP}

OBJS += Ioc/ioc_hotfix.o
Ioc/ioc_hotfix.o: \
    Ioc/ioc_hotfix.c \
    Ioc/ioc.h \
    Ioc/memspace.h \
    Infra/vend.h \
    ${R1000DEP}

OBJS += Ioc/ioc_interrupt.o
Ioc/ioc_interrupt.o: \
    Ioc/ioc_interrupt.c \
    Ioc/ioc.h \
    ${R1000DEP}

OBJS += Ioc/ioc_main.o
Ioc/ioc_main.o: \
    Ioc/ioc_main.c \
    Infra/vsb.h \
    Infra/vend.h \
    Ioc/memspace.h \
    ${R1000DEP}

OBJS += Ioc/ioc_memory.o
Infra/ioc_memory.o: \
    Ioc/ioc_memory.c \
    Ioc/memspace.h \
    ${R1000DEP}

OBJS += Ioc/ioc_memtrace.o
Ioc/ioc_memtrace.o: \
    Ioc/ioc_memtrace.c \
    Ioc/ioc.h \
    Ioc/memspace.h \
    ${R1000DEP}

OBJS += Ioc/ioc_mosart.o
Ioc/ioc_mosart.o: \
    Ioc/ioc_mosart.c \
    Ioc/ioc.h \
    Ioc/memspace.h \
    ${R1000DEP}

OBJS += Ioc/ioc_rtc.o
Ioc/ioc_rtc.o: \
    Ioc/ioc_rtc.c \
    Ioc/ioc.h \
    Ioc/memspace.h \
    ${R1000DEP}

OBJS += Ioc/ioc_sc.o
Ioc/ioc_sc.o: \
    Ioc/ioc_sc.c \
    Ioc/ioc_sc_68k20.hh \
    Ioc/ioc.h \
    ${R1000DEP}

OBJS += Ioc/ioc_scsi_ctl.o
Ioc/ioc_scsi_ctl.o: \
    Ioc/ioc_scsi_ctl.c \
    Ioc/ioc_scsi.h \
    Ioc/ioc.h \
    Ioc/memspace.h \
    ${R1000DEP}

OBJS += Ioc/ioc_scsi_dev.o
Ioc/ioc_scsi_dev.o: \
    Ioc/ioc_scsi_dev.c \
    Ioc/ioc_scsi.h \
    Ioc/ioc.h \
    ${R1000DEP}

OBJS += Ioc/ioc_syscall.o
Ioc/ioc_syscall.o: \
    Ioc/ioc_syscall.c \
    Ioc/ioc.h \
    Ioc/memspace.h \
    Infra/vsb.h \
    ${R1000DEP}

OBJS += Ioc/ioc_uart.o
Ioc/ioc_uart.o: \
    Ioc/ioc_uart.c \
    Ioc/ioc.h \
    Infra/elastic.h \
    ${R1000DEP}

OBJS += Ioc/_memcfg.o
Ioc/_memcfg.o: \
    Ioc/_memcfg.c \
    Ioc/_memcfg.h \
    Infra/vend.h \
    Infra/vqueue.h \
    Ioc/memspace.h

CLEANFILES += Ioc/_memcfg.c Ioc/_memcfg.h

Ioc/_memcfg.c Ioc/_memcfg.h: \
    Ioc/makemem.py \
    Ioc/makemem_class.py
	python3 Ioc/makemem.py
