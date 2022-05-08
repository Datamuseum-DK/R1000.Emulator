## See mkmk.sh
#

OBJS += Components/tables.o
Components/tables.o: Components/tables.c 
	${CC} -o Components/tables.o -c Components/tables.c

OBJS += Components/1489.o
Components/1489.o: Components/1489.cc \
    Components/1489.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/1489.o Components/1489.cc

OBJS += Components/2661B.o
Components/2661B.o: Components/2661B.cc \
    Components/2661B.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/2661B.o Components/2661B.cc

OBJS += Components/2681.o
Components/2681.o: Components/2681.cc \
    Components/2681.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/2681.o Components/2681.cc

OBJS += Components/28256.o
Components/28256.o: Components/28256.cc \
    Components/28256.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/28256.o Components/28256.cc

OBJS += Components/58167.o
Components/58167.o: Components/58167.cc \
    Components/58167.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/58167.o Components/58167.cc

OBJS += Components/68020.o
Components/68020.o: Components/68020.cc \
    Components/68020.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/68020.o Components/68020.cc

OBJS += Components/8051.o
Components/8051.o: Components/8051.cc \
    Components/8051.hh \
    Infra/context.h \
    Chassis/r1000sc.h \
    Diag/diagproc.h
	${SC_CC} -o Components/8051.o Components/8051.cc

OBJS += Components/CMDPAL.o
Components/CMDPAL.o: Components/CMDPAL.cc \
    Components/CMDPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/CMDPAL.o Components/CMDPAL.cc

OBJS += Components/DIBRPAL.o
Components/DIBRPAL.o: Components/DIBRPAL.cc \
    Components/DIBRPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/DIBRPAL.o Components/DIBRPAL.cc

OBJS += Components/DISTPAL.o
Components/DISTPAL.o: Components/DISTPAL.cc \
    Components/DISTPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/DISTPAL.o Components/DISTPAL.cc

OBJS += Components/DLY_35.o
Components/DLY_35.o: Components/DLY_35.cc \
    Components/DLY_35.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/DLY_35.o Components/DLY_35.cc

OBJS += Components/F245.o
Components/F245.o: Components/F245.cc \
    Components/F245.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F245.o Components/F245.cc

Components/F374.hh:: Components/F374_pins.hh
	@touch Components/F374.hh

OBJS += Components/F374.o
Components/F374.o: Components/F374.cc \
    Components/F374.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F374.o Components/F374.cc

OBJS += Components/F38.o
Components/F38.o: Components/F38.cc \
    Components/F38.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/F38.o Components/F38.cc

OBJS += Components/MARPAL.o
Components/MARPAL.o: Components/MARPAL.cc \
    Components/MARPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/MARPAL.o Components/MARPAL.cc

OBJS += Components/MUXLPAL.o
Components/MUXLPAL.o: Components/MUXLPAL.cc \
    Components/MUXLPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/MUXLPAL.o Components/MUXLPAL.cc

OBJS += Components/PHITPAL.o
Components/PHITPAL.o: Components/PHITPAL.cc \
    Components/PHITPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/PHITPAL.o Components/PHITPAL.cc

OBJS += Components/RDRPAL.o
Components/RDRPAL.o: Components/RDRPAL.cc \
    Components/RDRPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/RDRPAL.o Components/RDRPAL.cc

OBJS += Components/TPARPAL.o
Components/TPARPAL.o: Components/TPARPAL.cc \
    Components/TPARPAL.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/TPARPAL.o Components/TPARPAL.cc

Components/XADD8.hh:: Components/XADD8_pins.hh
	@touch Components/XADD8.hh
OBJS += Components/XADD8.o
Components/XADD8.o: Components/XADD8.cc Components/xbuf_doit.h \
    Components/XADD8.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XADD8.o Components/XADD8.cc

Components/XMUX232.hh:: Components/XMUX232_pins.hh
	@touch Components/XMUX232.hh

OBJS += Components/XMUX232.o
Components/XMUX232.o: Components/XMUX232.cc \
    Components/XMUX232.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XMUX232.o Components/XMUX232.cc
