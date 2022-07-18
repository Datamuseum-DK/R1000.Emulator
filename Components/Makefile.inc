## See mkmk.sh
#

OBJS += ${OBJDIR}/tables.o
${OBJDIR}/tables.o: Components/tables.c 
	${CC} ${CFLAGS} -c Components/tables.c -o ${OBJDIR}/tables.o

OBJS += ${OBJDIR}/8051.o
${OBJDIR}/8051.o: Components/8051.cc \
    Components/8051.hh \
    Infra/context.h \
    Chassis/r1000sc.h \
    Diag/diagproc.h
	${SC_CC} -c Components/8051.cc -o ${OBJDIR}/8051.o

Components/F374.hh:: Components/F374_pins.hh
	@touch Components/F374.hh

OBJS += ${OBJDIR}/F374.o
${OBJDIR}/F374.o: Components/F374.cc \
    Components/F374.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -c Components/F374.cc -o ${OBJDIR}/F374.o

OBJS += ${OBJDIR}/F38.o
${OBJDIR}/F38.o: Components/F38.cc \
    Components/F38.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -c Components/F38.cc -o ${OBJDIR}/F38.o
