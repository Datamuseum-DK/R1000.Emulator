
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
    Chassis/${SC_BRANCH}/planes.hh \
    Emu/${SC_BRANCH}/emu_board_pub.hh \
    Fiu/${SC_BRANCH}/fiu_board_pub.hh \
    Ioc/${SC_BRANCH}/ioc_board_pub.hh \
    Mem32/${SC_BRANCH}/mem32_board_pub.hh \
    Seq/${SC_BRANCH}/seq_board_pub.hh \
    Typ/${SC_BRANCH}/typ_board_pub.hh \
    Val/${SC_BRANCH}/val_board_pub.hh 
	${SC_CC} \
		-IEmu/${SC_BRANCH}/ \
		-IFiu/${SC_BRANCH}/ \
		-IIoc/${SC_BRANCH}/ \
		-IMem32/${SC_BRANCH}/ \
		-ISeq/${SC_BRANCH}/ \
		-ITyp/${SC_BRANCH}/ \
		-IVal/${SC_BRANCH}/ \
		-IChassis/${SC_BRANCH}/ \
		-o ${OBJDIR}/sc_main_thread.o \
		Chassis/sc_main_thread.cc

OBJS += ${OBJDIR}/sc_watchdog.o
${OBJDIR}/sc_watchdog.o: \
    Infra/r1000.h \
    Infra/context.h \
    Diag/diagproc.h \
    Chassis/r1000sc_priv.h
	${CC} ${CFLAGS} -c Chassis/sc_watchdog.c -o ${OBJDIR}/sc_watchdog.o

