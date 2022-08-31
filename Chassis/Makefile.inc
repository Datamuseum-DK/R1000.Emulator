
OBJS += ${OBJDIR}/sc.o
${OBJDIR}/sc.o: \
    Chassis/sc.c \
    Infra/r1000.h \
    Infra/vqueue.h \
    Chassis/r1000sc.h \
    Chassis/r1000sc_priv.h
	${CC} ${CFLAGS} -c Chassis/sc.c -o ${OBJDIR}/sc.o

OBJS += ${OBJDIR}/sc_main_thread.o
${OBJDIR}/sc_main_thread.o: \
    Chassis/sc_main_thread.cc \
    Chassis/r1000sc.h \
    Chassis/r1000sc_priv.h \
    ${WORKDIR}/Chassis/${BRANCH}/planes.hh \
    ${WORKDIR}/Emu/${BRANCH}/emu_board_pub.hh \
    ${WORKDIR}/Fiu/${BRANCH}/fiu_board_pub.hh \
    ${WORKDIR}/Ioc/${BRANCH}/ioc_board_pub.hh \
    ${WORKDIR}/Mem32/${BRANCH}/mem32_board_pub.hh \
    ${WORKDIR}/Seq/${BRANCH}/seq_board_pub.hh \
    ${WORKDIR}/Typ/${BRANCH}/typ_board_pub.hh \
    ${WORKDIR}/Val/${BRANCH}/val_board_pub.hh 
	${SC_CC} \
		-I${WORKDIR}/Emu/${BRANCH}/ \
		-I${WORKDIR}/Fiu/${BRANCH}/ \
		-I${WORKDIR}/Ioc/${BRANCH}/ \
		-I${WORKDIR}/Mem32/${BRANCH}/ \
		-I${WORKDIR}/Seq/${BRANCH}/ \
		-I${WORKDIR}/Typ/${BRANCH}/ \
		-I${WORKDIR}/Val/${BRANCH}/ \
		-I${WORKDIR}/Chassis/${BRANCH}/ \
		-o ${OBJDIR}/sc_main_thread.o \
		Chassis/sc_main_thread.cc

OBJS += ${OBJDIR}/sc_watchdog.o
${OBJDIR}/sc_watchdog.o: \
    Chassis/sc_watchdog.c \
    Infra/r1000.h \
    Infra/context.h \
    Diag/diagproc.h \
    Chassis/r1000sc_priv.h
	${CC} ${CFLAGS} -c Chassis/sc_watchdog.c -o ${OBJDIR}/sc_watchdog.o

