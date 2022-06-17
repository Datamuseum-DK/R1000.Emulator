## See mkmk.sh
#

OBJS += Components/tables.o
Components/tables.o: Components/tables.c 
	${CC} -o Components/tables.o -c Components/tables.c

OBJS += Components/8051.o
Components/8051.o: Components/8051.cc \
    Components/8051.hh \
    Infra/context.h \
    Chassis/r1000sc.h \
    Diag/diagproc.h
	${SC_CC} -o Components/8051.o Components/8051.cc

OBJS += Components/DLY_35.o
Components/DLY_35.o: Components/DLY_35.cc \
    Components/DLY_35.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/DLY_35.o Components/DLY_35.cc

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

Components/XMUX232.hh:: Components/XMUX232_pins.hh
	@touch Components/XMUX232.hh

OBJS += Components/XMUX232.o
Components/XMUX232.o: Components/XMUX232.cc \
    Components/XMUX232.hh \
    Infra/context.h \
    Chassis/r1000sc.h
	${SC_CC} -o Components/XMUX232.o Components/XMUX232.cc
