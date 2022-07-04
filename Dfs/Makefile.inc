OBJS    += Dfs/dfs.o
Dfs/dfs.o: \
    Dfs/dfs.c \
    Infra/vend.h \
    Iop/iop_scsi.h \
    ${R1000DEP}

