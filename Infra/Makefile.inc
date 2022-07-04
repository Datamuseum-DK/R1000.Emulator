
OBJS += Infra/callout.o
Infra/callout.o: \
    Infra/callout.c \
    ${R1000DEP}

OBJS += Infra/cli.o
Infra/cli.o: \
    Infra/cli.c \
    Infra/vav.h \
    Iop/iop.h \
    ${R1000DEP}

OBJS += Infra/context.o
Infra/context.o: \
    Infra/context.c \
    Infra/context.h \
    ${R1000DEP}

OBJS += Infra/elastic.o
Infra/elastic.o: \
    Infra/elastic.c \
    Infra/elastic.h \
    ${R1000DEP}

OBJS += Infra/elastic_fd.o
Infra/elastic_fd.o: \
    Infra/elastic_fd.c \
    Infra/elastic.h \
    ${R1000DEP}

OBJS += Infra/elastic_match.o
Infra/elastic_elastic.o: \
    Infra/elastic_elastic.c \
    Infra/elastic.h \
    ${R1000DEP}

OBJS += Infra/firmware.o
Infra/firmware.o: \
    ${R1000DEP}

OBJS += Infra/elastic_tcp.o
Infra/elastic_tcp.o: \
    Infra/elastic_tcp.c \
    Infra/elastic.h \
    ${R1000DEP}

OBJS += Infra/main.o
Infra/main.o: \
    Infra/main.c \
    Infra/vsb.h \
    Iop/iop.h \
    ${R1000DEP}

OBJS += Infra/rpn.o
Infra/rpn.o: \
    Infra/rpn.c \
    Infra/vsb.h \
    ${R1000DEP}

OBJS += Infra/trace.o
Infra/trace.o: \
    Infra/trace.c \
    Infra/vsb.h \
    ${R1000DEP}

OBJS += Infra/vav.o
Infra/vav.o: \
    Infra/vav.c \
    Infra/vav.h

OBJS += Infra/vsb.o
Infra/vsb.o: \
    Infra/vsb.c \
    Infra/vsb.h
