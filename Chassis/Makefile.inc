
OBJS += Chassis/sc.o
Chassis/sc.o: \
    Chassis/sc.c \
    Infra/r1000.h \
    Infra/vqueue.h \
    Chassis/r1000sc.h \
    Chassis/r1000sc_priv.h

OBJS += Chassis/planes.o
Chassis/planes.o: \
    Chassis/planes.cc \
    Chassis/planes.hh \
    Chassis/plane_tbl.h
	${SC_CC} -o Chassis/planes.o Chassis/planes.cc

OBJS += Chassis/sc_main_thread_${SC_BRANCH}.o
Chassis/sc_main_thread_${SC_BRANCH}.o: \
    Chassis/sc_main_thread.cc \
    Chassis/planes.hh \
    Chassis/plane_tbl.h \
    Chassis/r1000sc.h \
    Chassis/r1000sc_priv.h \
    Fiu/${SC_BRANCH}/fiu_board_pub.hh \
    Ioc/${SC_BRANCH}/ioc_board_pub.hh \
    Mem32/${SC_BRANCH}/mem32_board_pub.hh \
    Seq/${SC_BRANCH}/seq_board_pub.hh \
    Typ/${SC_BRANCH}/typ_board_pub.hh \
    Val/${SC_BRANCH}/val_board_pub.hh 
	${SC_CC} \
		-IFiu/${SC_BRANCH}/ \
		-IIoc/${SC_BRANCH}/ \
		-IMem32/${SC_BRANCH}/ \
		-ISeq/${SC_BRANCH}/ \
		-ITyp/${SC_BRANCH}/ \
		-IVal/${SC_BRANCH}/ \
		-o Chassis/sc_main_thread_${SC_BRANCH}.o \
		Chassis/sc_main_thread.cc

OBJS += Chassis/sc_watchdog.o
