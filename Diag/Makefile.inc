

OBJS    += Diag/diagbus.o
Diag/diagbus.o: \
    Diag/diagbus.c \
    Infra/elastic.h \
    Diag/diag.h \
    ${R1000DEP}

OBJS    += Diag/diagproc.o
Diag/diagproc.o: \
    Diag/diagproc.c \
    Chassis/r1000sc.h \
    Chassis/r1000sc_priv.h \
    Diag/diagproc.h \
    Diag/i8052_emul.h \
    Infra/elastic.h \
    Diag/diag.h \
    ${R1000DEP}

OBJS    += Diag/dummy_diproc.o
Diag/dummy_diproc.o: \
    Diag/dummy_diproc.c \
    Infra/vsb.h \
    Infra/elastic.h \
    Diag/diag.h \
    ${R1000DEP}

OBJS    += Diag/experiment.o
Diag/experiment.o: \
    Diag/experiment.c \
    Infra/elastic.h \
    Infra/vqueue.h \
    Diag/diag.h \
    ${R1000DEP}

OBJS    += Diag/i8052_emul.o
Diag/i8052_emul.o: \
    Diag/i8052_emul.c \
    Diag/i8052_emul.h
