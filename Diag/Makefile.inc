

OBJS    += diagbus.o
diagbus.o: \
    Diag/diagbus.c \
    Infra/r1000.h \
    Infra/elastic.h \
    Diag/diag.h

OBJS    += diagproc.o
diagproc.o: \
    Diag/diagproc.c \
    Infra/r1000.h \
    Chassis/r1000sc.h \
    Chassis/r1000sc_priv.h \
    Diag/diagproc.h \
    Diag/i8052_emul.h \
    Infra/elastic.h \
    Diag/diag.h

OBJS    += dummy_diproc.o
dummy_diproc.o: \
    Diag/dummy_diproc.c \
    Infra/r1000.h \
    Infra/vsb.h \
    Infra/elastic.h \
    Diag/diag.h

OBJS    += experiment.o
experiment.o: \
    Diag/experiment.c \
    Infra/r1000.h \
    Infra/elastic.h \
    Infra/vqueue.h \
    Diag/diag.h


OBJS    += i8052_emul.o
i8052_emul.o: \
    Diag/i8052_emul.c \
    Diag/i8052_emul.h
