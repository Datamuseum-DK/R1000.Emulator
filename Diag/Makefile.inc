

OBJS    += diagbus.o
diagbus.o:	${CLI_INCL} Diag/diagbus.c

OBJS    += diagproc.o
diagproc.o:	${CLI_INCL} Diag/diagproc.c

OBJS    += experiment.o
experiment.o:	${CLI_INCL} Diag/experiment.c

OBJS    += i8052.o
i8052.o:	${CLI_INCL} Diag/i8052.c

OBJS    += i8052_emul.o
i8052_emul.o:	${CLI_INCL} Diag/i8052_emul.c
