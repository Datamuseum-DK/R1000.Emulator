
OBJS += Iop/iop_cli.o
Iop/iop_cli.o: \
    Iop/iop_cli.c \
    Iop/iop.h \
    ${R1000DEP}

OBJS += Iop/iop_debug.o
Iop/iop_debug.o: \
    Iop/iop_debug.c \
    Musashi/m68k.h \
    Iop/iop.h \
    Infra/vsb.h \
    Iop/memspace.h \
    Iop/_memcfg.h \
    ${R1000DEP}

OBJS += Iop/iop_duart.o
Iop/iop_duart.o: \
    Iop/iop_duart.c \
    Iop/iop.h \
    Infra/elastic.h \
    Iop/memspace.h \
    Diag/diag.h \
    ${R1000DEP}

OBJS += Iop/iop_eeproms.o
Iop/iop_eeproms.o: \
    Iop/iop_eeproms.c \
    Iop/iop.h \
    Iop/memspace.h \
    Infra/vend.h \
    Chassis/r1000sc.h \
    ${R1000DEP}

OBJS += Iop/iop_pnlreg.o
Iop/iop_pnlreg.o: \
    Iop/iop_pnlreg.c \
    Iop/memspace.h \
    Infra/vend.h \
    ${R1000DEP}

OBJS += Iop/iop_hotfix.o
Iop/iop_hotfix.o: \
    Iop/iop_hotfix.c \
    Iop/iop.h \
    Iop/memspace.h \
    Infra/vend.h \
    ${R1000DEP}

OBJS += Iop/iop_interrupt.o
Iop/iop_interrupt.o: \
    Iop/iop_interrupt.c \
    Iop/iop.h \
    ${R1000DEP}

OBJS += Iop/iop_main.o
Iop/iop_main.o: \
    Iop/iop_main.c \
    Infra/vsb.h \
    Infra/vend.h \
    Iop/memspace.h \
    ${R1000DEP}

OBJS += Iop/iop_memory.o
Infra/iop_memory.o: \
    Iop/iop_memory.c \
    Iop/memspace.h \
    ${R1000DEP}

OBJS += Iop/iop_memtrace.o
Iop/iop_memtrace.o: \
    Iop/iop_memtrace.c \
    Iop/iop.h \
    Iop/memspace.h \
    ${R1000DEP}

OBJS += Iop/iop_mosart.o
Iop/iop_mosart.o: \
    Iop/iop_mosart.c \
    Iop/iop.h \
    Iop/memspace.h \
    ${R1000DEP}

OBJS += Iop/iop_rtc.o
Iop/iop_rtc.o: \
    Iop/iop_rtc.c \
    Iop/iop.h \
    Iop/memspace.h \
    ${R1000DEP}

OBJS += Iop/iop_sc.o
Iop/iop_sc.o: \
    Iop/iop_sc.c \
    Iop/iop_sc_68k20.hh \
    Iop/iop.h \
    Chassis/r1000sc_priv.h \
    ${R1000DEP}

OBJS += Iop/iop_scsi_ctl.o
Iop/iop_scsi_ctl.o: \
    Iop/iop_scsi_ctl.c \
    Iop/iop_scsi.h \
    Iop/iop.h \
    Iop/memspace.h \
    ${R1000DEP}

OBJS += Iop/iop_scsi_dev.o
Iop/iop_scsi_dev.o: \
    Iop/iop_scsi_dev.c \
    Iop/iop_scsi.h \
    Iop/iop.h \
    ${R1000DEP}

OBJS += Iop/iop_syscall.o
Iop/iop_syscall.o: \
    Iop/iop_syscall.c \
    Iop/iop.h \
    Iop/memspace.h \
    Infra/vsb.h \
    ${R1000DEP}

OBJS += Iop/iop_uart.o
Iop/iop_uart.o: \
    Iop/iop_uart.c \
    Iop/iop.h \
    Infra/elastic.h \
    ${R1000DEP}

OBJS += Iop/_memcfg.o
Iop/_memcfg.o: \
    Iop/_memcfg.c \
    Iop/_memcfg.h \
    Infra/vend.h \
    Infra/vqueue.h \
    Iop/memspace.h

CLEANFILES += Iop/_memcfg.c Iop/_memcfg.h

Iop/_memcfg.c Iop/_memcfg.h: \
    Iop/makemem.py \
    Iop/makemem_class.py
	python3 Iop/makemem.py
